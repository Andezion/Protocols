use std::net::SocketAddr;
use sctp_rs::{Socket, SocketToAssociation, SendData, NotificationOrData};
use sctp_rs::Notification;

// ассинхронность потому что библиотека sctp_rs использует tokio для асинхронного ввода-вывода
#[tokio::main]
async fn main() -> std::io::Result<()> {
    let addr: SocketAddr = "127.0.0.1:8090".parse().expect("invalid addr");
    let sock = Socket::new_v4(SocketToAssociation::OneToOne)?;
    sock.bind(addr)?;
    let listener = sock.listen(1)?;

    println!("SCTP echo server listening on {}", addr);

    let (conn, peer) = listener.accept().await?;
    println!("Client connected: {}", peer);

    loop {
        match conn.sctp_recv().await? {
            NotificationOrData::Data(data) => {
                let payload = data.payload.clone();
                println!("Received: {}", String::from_utf8_lossy(&payload));
                conn.sctp_send(SendData { payload, snd_info: None }).await?;
                println!("Echoed back");
            }
            NotificationOrData::Notification(notif) => {
                println!("Notification: {:?}", notif);
                if let Notification::Shutdown(_) = notif {
                    println!("Shutdown notification received, exiting");
                    break;
                }
            }
        }
    }

    Ok(())
}