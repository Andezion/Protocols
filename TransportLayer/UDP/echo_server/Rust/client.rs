use std::io::{self, Read, Write};
use std::net::UdpSocket;

fn main() -> io::Result<()> {
    let addr = "127:0.0.1:8090";
    let socket = UdpSocket::bind(addr)?;
    println!("Connected to server at {}", addr);
}