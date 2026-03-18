use std::io::{self, Read, Write};
use std::net::{TcpListener, TcpStream};

fn main() -> io::Result<()> {
    // занимаем место 
    let listener = TcpListener::bind("0.0.0.0:8090")?;
    println!("Multi-client echo server listening on 0.0.0.0:8090");
    
    loop { // и тут снова разница - у нас просто бесконечный цикл
        let (mut stream, addr) = listener.accept()?;
        println!("Client connected: {}", addr);
        
        // Обрабатываем клиента в отдельном потоке
        std::thread::spawn(move || {    
            let mut buf = [0u8; 1024];
            // Читаем данные от клиента в цикле, пока он не отключится
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
        });
    }
    
    Ok(())
}