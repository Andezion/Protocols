use std::io::{self, Read, Write};
use std::net::TcpListener;

fn main() -> io::Result<()> {
    let listener = TcpListener::bind("0.0.0.0:8090")?;
    println!("One-shot echo server listening on 0.0.0.0:8090");

    // Accept exactly one connection, read once, echo once, exit.
    let (mut stream, addr) = listener.accept()?;
    println!("Client connected: {}", addr);

    let mut buf = [0u8; 1024];
    let n = stream.read(&mut buf)?;
    if n == 0 {
        println!("Client sent no data");
        return Ok(());
    }

    println!("Received: {}", String::from_utf8_lossy(&buf[..n]));
    stream.write_all(&buf[..n])?;
    println!("Echoed back and exiting.");

    Ok(())
}
