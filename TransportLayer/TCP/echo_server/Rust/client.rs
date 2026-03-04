use std::{io, net::{TcpListener, TcpStream}, thread};

fn main() -> io::Result<()> {
    let addr = "127.0.0.1:8090";
    let stream = TcpStream::connect(addr)?;
    println!("Connected to server at {}", addr);
    Ok(())
}