use std::io;
use std::net::UdpSocket;

fn main() -> io::Result<()> {
    let socket = UdpSocket::bind("0.0.0.0:8090")?;
    println!("One-shot UDP echo server listening on 0.0.0.0:8090");

    let mut buf = [0u8; 1024];
    let (n, src) = socket.recv_from(&mut buf)?;
    
    println!("Received {} bytes from {}", n, src);
    println!("Received: {}", String::from_utf8_lossy(&buf[..n]));

    socket.send_to(&buf[..n], src)?;
    println!("Echoed back and exiting.");

    Ok(())
}