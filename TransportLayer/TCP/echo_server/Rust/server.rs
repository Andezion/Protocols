use std::io::{self, Read, Write};
use std::net::TcpListener;

fn main() -> io::Result<()> {
    // Биндим сервер на порт 8090, слушая на всех интерфейсах
    let listener = TcpListener::bind("0.0.0.0:8090")?;
    println!("One-shot echo server listening on 0.0.0.0:8090");

    // Принимаем одно соединение от клиента
    let (mut stream, addr) = listener.accept()?;
    println!("Client connected: {}", addr);

    // Читаем данные от клиента
    let mut buf = [0u8; 1024];
    let n = stream.read(&mut buf)?;
    if n == 0 {
        println!("Client sent no data");
        return Ok(());
    }

    // Выводим полученные данные и отправляем их обратно клиенту
    println!("Received: {}", String::from_utf8_lossy(&buf[..n]));
    stream.write_all(&buf[..n])?;
    println!("Echoed back and exiting.");

    Ok(())
}
