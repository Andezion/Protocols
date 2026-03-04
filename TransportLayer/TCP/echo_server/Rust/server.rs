use std::{io, net::{TcpListener, TcpStream}, thread};

fn main() -> io::Result<()> {
    let listener = TcpListener::bind(":8090")?;

    println!("Echo server is running on :8090");

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

}

