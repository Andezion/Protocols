use std::io::{self, Read, Write};
use std::net::UdpListener;

fn main() -> io::Result<()> {
    let listener = UdpListener::bind("0.0.0.0:8090")?;S
    println!("One-shot echo server listening on 0.0.0.0:8090");
}