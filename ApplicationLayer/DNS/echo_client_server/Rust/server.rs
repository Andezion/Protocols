use std::collections::HashMap;
use std::env;
use std::io;
use std::net::UdpSocket;

fn main() -> io::Result<()> {
    let bind_addr = env::args()
        .nth(1)
        .unwrap_or_else(|| String::from("127.0.0.1:5300"));

    let socket = UdpSocket::bind(&bind_addr)?;
    let records = HashMap::from([
        (String::from("alice.local"), String::from("127.0.0.1")),
        (String::from("bob.local"), String::from("127.0.0.1")),
    ]);

    println!("DNS demo server listening on {bind_addr}");

    let mut buffer = [0_u8; 1024];

    loop {
        let (received, peer) = socket.recv_from(&mut buffer)?;
        let request = String::from_utf8_lossy(&buffer[..received]).trim().to_string();

        let response = if let Some(name) = request.strip_prefix("QUERY ") {
            match records.get(name.trim()) {
                Some(ip) => format!("OK {ip}"),
                None => String::from("ERR unknown name"),
            }
        } else {
            String::from("ERR expected: QUERY <name>")
        };

        socket.send_to(response.as_bytes(), peer)?;
    }
}