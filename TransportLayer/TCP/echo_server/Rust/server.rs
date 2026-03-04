use std::{io::{self, Read, Write}, net::{TcpListener, TcpStream}, thread};

fn main() -> io::Result<()> {
    let listener = TcpListener::bind("127.0.0.1:8090")?;

    println!("Echo server is running on 127.0.0.1:8090");

    for stream in listener.incoming() {
        match stream {
            Ok(stream) => {
                println!("New client connected: {}", stream.peer_addr()?);
                thread::spawn(move || handle_client(stream));
            }
            Err(e) => {
                eprintln!("Failed to accept client: {}", e);
            }
        }
    }

    Ok(())
}

fn handle_client(mut stream: TcpStream) {
    let peer = match stream.peer_addr() {
        Ok(a) => a.to_string(),
        Err(_) => String::from("unknown"),
    };

    let mut buf = [0u8; 1024];
    loop {
        match stream.read(&mut buf) {
            Ok(0) => {
                println!("Client {} disconnected", peer);
                break;
            }
            Ok(n) => {
                if let Err(e) = stream.write_all(&buf[..n]) {
                    eprintln!("Failed to write to {}: {}", peer, e);
                    break;
                }
            }
            Err(e) => {
                eprintln!("Read error from {}: {}", peer, e);
                break;
            }
        }
    }
}

