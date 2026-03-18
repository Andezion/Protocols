use std::io::{self, Read, Write};
use std::net::{TcpListener, TcpStream};

fn main() -> io::Result<()> {
    let listener = TcpListener::bind("0.0.0.0:8090")?;
    println!("Multi-client echo server listening on 0.0.0.0:8090");
    Ok(())
}