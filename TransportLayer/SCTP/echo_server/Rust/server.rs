use std::net::SocketAddr;
use sctp_rs::{Socket, SocketToAssociation, SendData, NotificationOrData};
use sctp_rs::Notification;

// ассинхронность потому что библиотека sctp_rs использует tokio для асинхронного ввода-вывода
#[tokio::main]
async fn main() -> std::io::Result<()> {
    // наш адрес для прослушивания входящих соединений
    let addr: SocketAddr = "127.0.0.1:8090".parse().expect("invalid addr");
    // создаем SCTP сокет и привязываем его к адресу
    let sock = Socket::new_v4(SocketToAssociation::OneToOne)?;
    sock.bind(addr)?;

    // начинаем прослушивание входящих соединений, максимальная очередь 10
    let listener = sock.listen(10)?;

    println!("SCTP echo server listening on {}", addr);

    // бесконечный цикл для принятия и обработки входящих соединений
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