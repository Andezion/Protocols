use std::net::SocketAddr;
use sctp_rs::{Socket, SocketToAssociation, SendData, NotificationOrData};

#[tokio::main]
async fn main() -> std::io::Result<()> {
    let addr: SocketAddr = "127.0.0.1:8090".parse().unwrap();
        let sock = Socket::new_v4(SocketToAssociation::OneToOne)?;
        let (conn, _assoc) = sock.connect(addr).await?;
        println!("Connected to server at {}", addr);

        conn.sctp_send(SendData { payload: b"Hello from Rust client\n".to_vec(), snd_info: None }).await?;

        if let NotificationOrData::Data(msg) = conn.sctp_recv().await? {
            println!("Received echo: {}", String::from_utf8_lossy(&msg.payload));
        }

    Ok(())
}