use std::io::{self};
use std::net::UdpSocket;

fn main() -> io::Result<()> {
    // Биндим сокет на локальный адрес и порт 0 (выбирается автоматически)
    let addr = "127.0.0.1:8090";
    let socket = UdpSocket::bind(addr)?;
    println!("Connected to server at {}", addr);

    let msg = b"hello from rust client\n";
    socket.send_to(msg, addr)?; // Отправляем сообщение на сервер

    let mut buf = [0u8; 1024];
    let n = socket.recv(&mut buf)?; // Получаем ответ от сервера
    println!("Received echo: {}", String::from_utf8_lossy(&buf[..n]));

    Ok(())
}