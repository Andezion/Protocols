package yggmini

import (
	"crypto/ed25519"
	"crypto/rand"
	"crypto/sha512"
	"encoding/binary"
	"fmt"
	"io"
	"net"
	"net/netip"

	"filippo.io/edwards25519"
	"golang.org/x/crypto/nacl/box"
)

const (
	VersionMajor = 0
	VersionMinor = 1

	metaMagic = "YGGm"
	// magic(4) + major(1) + minor(1) + ed25519 pubkey(32)
	metaSize = 4 + 1 + 1 + ed25519.PublicKeySize
)

// Node - identity узла, типа ключи Ed25519 (подписи/личность) и производные от
// них ключи Curve25519 (используются для NaCl box), плюс адрес, выведенный из pubkey
type Node struct {
	EdPub  ed25519.PublicKey
	EdPriv ed25519.PrivateKey

	BoxPub  [32]byte
	BoxPriv [32]byte

	Addr [16]byte
}

// NewNode генерирует пару Ed25519-ключей и выводит из неё пару
// Curve25519-ключей и crypto-адрес узла
func NewNode() (*Node, error) {
	pub, priv, err := ed25519.GenerateKey(rand.Reader)
	if err != nil {
		return nil, fmt.Errorf("generate ed25519 key: %w", err)
	}

	boxPub, boxPriv, err := edToBoxKeys(pub, priv)
	if err != nil {
		return nil, err
	}

	return &Node{
		EdPub:   pub,
		EdPriv:  priv,
		BoxPub:  boxPub,
		BoxPriv: boxPriv,
		Addr:    AddrForKey(pub),
	}, nil
}

// edToBoxKeys конвертирует identity-ключи Ed25519 в ключи Curve25519 для
// NaCl box - ровно то, что описано в README про Yggdrasil
func edToBoxKeys(pub ed25519.PublicKey, priv ed25519.PrivateKey) (boxPub, boxPriv [32]byte, err error) {
	p, err := new(edwards25519.Point).SetBytes(pub)
	if err != nil {
		return boxPub, boxPriv, fmt.Errorf("invalid ed25519 pubkey: %w", err)
	}
	copy(boxPub[:], p.BytesMontgomery())

	h := sha512.Sum512(priv.Seed())
	copy(boxPriv[:], h[:32])
	boxPriv[0] &= 248
	boxPriv[31] &= 127
	boxPriv[31] |= 64

	return boxPub, boxPriv, nil
}

// BoxPubForKey выводит Curve25519 box-ключ пира из его публичного
// Ed25519-ключа - нужно только для проверки чужих сообщений, приватный ключ
// пира нам, разумеется, не известен
func BoxPubForKey(pub ed25519.PublicKey) ([32]byte, error) {
	var out [32]byte
	p, err := new(edwards25519.Point).SetBytes(pub)
	if err != nil {
		return out, fmt.Errorf("invalid ed25519 pubkey: %w", err)
	}
	copy(out[:], p.BytesMontgomery())
	return out, nil
}

// AddrForKey - порт AddrForKey() из src/address/address.go настоящего
// yggdrasil-go, адрес начинается с префикса 0x02, следующий байт - число
// единичных битов подряд в начале побитовой инверсии ключа, дальше - хвост
// инверсии ключа
func AddrForKey(pub ed25519.PublicKey) [16]byte {
	var inv [ed25519.PublicKeySize]byte
	for i := range inv {
		inv[i] = ^pub[i]
	}

	var addr [16]byte
	tail := make([]byte, 0, len(inv))
	done := false
	var ones byte
	var bits byte
	nBits := 0

	for idx := 0; idx < 8*len(inv); idx++ {
		bit := (inv[idx/8] & (0x80 >> uint(idx%8))) >> uint(7-(idx%8))
		switch {
		case !done && bit != 0:
			ones++
			continue
		case !done && bit == 0:
			done = true
			continue
		}
		bits = (bits << 1) | bit
		nBits++
		if nBits == 8 {
			nBits = 0
			tail = append(tail, bits)
		}
	}

	addr[0] = 0x02
	addr[1] = ones
	copy(addr[2:], tail)
	return addr
}

// AddrString форматирует адрес узла в обычную IPv6-строку вида "201:85c5:..."
func AddrString(addr [16]byte) string {
	return netip.AddrFrom16(addr).String()
}

// SendMetadata отправляет по соединению пакет metadata (версия +
// Ed25519-identity) - первый шаг настоящего пиринга в Yggdrasil
func SendMetadata(conn net.Conn, n *Node) error {
	buf := make([]byte, metaSize)
	copy(buf[0:4], metaMagic)
	buf[4] = VersionMajor
	buf[5] = VersionMinor
	copy(buf[6:], n.EdPub)
	_, err := conn.Write(buf)
	return err
}

// RecvMetadata принимает и разбирает пакет metadata, присланный пиром
func RecvMetadata(conn net.Conn) (ed25519.PublicKey, error) {
	buf := make([]byte, metaSize)
	if _, err := io.ReadFull(conn, buf); err != nil {
		return nil, fmt.Errorf("read metadata: %w", err)
	}
	if string(buf[0:4]) != metaMagic {
		return nil, fmt.Errorf("bad magic %q", buf[0:4])
	}
	if buf[4] != VersionMajor {
		return nil, fmt.Errorf("incompatible major version %d", buf[4])
	}

	pub := make([]byte, ed25519.PublicKeySize)
	copy(pub, buf[6:])
	return pub, nil
}

// SendEncrypted шифрует сообщение для пира через NaCl box
// (X25519 + XSalsa20-Poly1305) и отправляет кадром
// [uint16 длина][nonce 24 байта][шифротекст]
func SendEncrypted(conn net.Conn, n *Node, peerBoxPub *[32]byte, msg []byte) error {
	var nonce [24]byte
	if _, err := rand.Read(nonce[:]); err != nil {
		return fmt.Errorf("generate nonce: %w", err)
	}

	sealed := box.Seal(nil, msg, &nonce, peerBoxPub, &n.BoxPriv)
	frame := append(nonce[:], sealed...)
	if len(frame) > 0xFFFF {
		return fmt.Errorf("frame too large: %d bytes", len(frame))
	}

	lenPrefix := make([]byte, 2)
	binary.BigEndian.PutUint16(lenPrefix, uint16(len(frame)))

	if _, err := conn.Write(lenPrefix); err != nil {
		return err
	}
	_, err := conn.Write(frame)
	return err
}

// RecvEncrypted принимает и расшифровывает такой кадр
func RecvEncrypted(conn net.Conn, n *Node, peerBoxPub *[32]byte) ([]byte, error) {
	lenPrefix := make([]byte, 2)
	if _, err := io.ReadFull(conn, lenPrefix); err != nil {
		return nil, fmt.Errorf("read frame length: %w", err)
	}
	frameLen := binary.BigEndian.Uint16(lenPrefix)
	if int(frameLen) < 24+box.Overhead {
		return nil, fmt.Errorf("frame too short: %d bytes", frameLen)
	}

	frame := make([]byte, frameLen)
	if _, err := io.ReadFull(conn, frame); err != nil {
		return nil, fmt.Errorf("read frame: %w", err)
	}

	var nonce [24]byte
	copy(nonce[:], frame[:24])

	plain, ok := box.Open(nil, frame[24:], &nonce, peerBoxPub, &n.BoxPriv)
	if !ok {
		return nil, fmt.Errorf("decryption/authentication failed")
	}
	return plain, nil
}
