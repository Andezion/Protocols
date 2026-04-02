use std::io::{self, Read, Write};
use std::net::TcpListener;

fn main() -> std::io::Result<()> {
    let client = sctp_rs::Socket::new_v4(sctp_rs::SocketToAssociation::OneToOne)?;

    client.bind("127.0.0.1:3000")?;
    client.listen(1)?;
    
    println!("SCTP echo server listening on 3000 port");

    let (mut stream, addr) = client.accept()?;
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