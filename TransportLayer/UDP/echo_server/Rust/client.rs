use std::io::{self};
use std::net::UdpSocket;

fn main() -> io::Result<()> {
    let addr = "127.0.0.1:8090";
    let socket = UdpSocket::bind(addr)?;
    println!("Connected to server at {}", addr);

    let msg = b"hello from rust client\n";
    socket.send_to(msg, addr)?;

    let mut buf = [0u8; 1024];
    let n = socket.recv(&mut buf)?;
    println!("Received echo: {}", String::from_utf8_lossy(&buf[..n]));

    Ok(())
}