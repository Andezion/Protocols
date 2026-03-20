use std::io::{self, Read, Write};
use std::net::TcpStream;


fn main() -> io::Result<()> {
    let addr = "127.0.0.1:8090";
    let mut stream = TcpStream::connect(addr)?;
    println!("Connected to server at {}", addr);

    let msg = b"hello from rust client\n";
    stream.write_all(msg)?; // Отправляем сообщение серверу

    // Читаем ответ от сервера
    let mut buf = [0u8; 1024];
    let n = stream.read(&mut buf)?;
    println!("Received echo: {}", String::from_utf8_lossy(&buf[..n]));

    Ok(())
}