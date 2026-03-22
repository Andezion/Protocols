use std::io;
use std::net::UdpSocket;

fn main() -> io::Result<()> {
    // Биндим сокет на все интерфейсы и порт 8090
    let socket = UdpSocket::bind("0.0.0.0:8090")?;
    println!("UDP echo server listening on 0.0.0.0:8090");

    let mut buf = [0u8; 1024];
    loop {
        let (n, src) = socket.recv_from(&mut buf)?; // Получаем данные и адрес отправителя
        
        println!("Received {} bytes from {}", n, src);
        println!("Received: {}", String::from_utf8_lossy(&buf[..n]));

        socket.send_to(&buf[..n], src)?; // Отправляем обратно полученные данные
        println!("Echoed back to {}", src);
    }
}