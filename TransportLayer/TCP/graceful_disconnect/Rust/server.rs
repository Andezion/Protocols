use std::io::{self, Read, Write};
use std::net::{TcpListener, TcpStream};

fn main() -> io::Result<()> {
    let listener = TcpListener::bind("0.0.0.0:8090")?;
    println!("Echo server listening on 0.0.0.0:8090");
    
    loop {
        let (mut stream, addr) = listener.accept()?;
        println!("Client connected: {}", addr);
        
        std::thread::spawn(move || {
            let mut buf = [0u8; 1024];
            loop {
                let n = match stream.read(&mut buf) {
                    Ok(0) => {
                        println!("Client {} disconnected", addr);
                        break;
                    },
                    Ok(n) => n,
                    Err(e) => {
                        eprintln!("Error reading from client {}: {}", addr, e);
                        break;
                    }
                };
                println!("Received from {}: {}", addr, String::from_utf8_lossy(&buf[..n]));
                if let Err(e) = stream.write_all(&buf[..n]) {
                    eprintln!("Error writing to client {}: {}", addr, e);
                    break;
                }
            }
            // Явное закрытие соединения
            if let Err(e) = stream.shutdown(std::net::Shutdown::Both) {
                eprintln!("Error shutting down connection for {}: {}", addr, e);
            }
        });
    }

    listener.close()?;
}