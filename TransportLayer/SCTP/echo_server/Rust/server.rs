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
        // ждем данных от клиента и обрабатываем их
        match conn.sctp_recv().await? {
            // если получили данные, то выводим их и отправляем обратно клиенту
            NotificationOrData::Data(data) => {
                // клонируем данные, так как они будут перемещены при отправке обратно
                let payload = data.payload.clone();
                println!("Received: {}", String::from_utf8_lossy(&payload));
                // отправляем данные обратно клиенту
                conn.sctp_send(SendData { payload, snd_info: None }).await?;
                println!("Echoed back");
            }
            // если получили уведомление, то выводим его и проверяем, не является ли оно уведомлением 
            // о завершении работы
            NotificationOrData::Notification(notif) => {
                println!("Notification: {:?}", notif);
                // если получили уведомление о завершении работы, то выходим из цикла и завершаем сервер
                if let Notification::Shutdown(_) = notif {
                    println!("Shutdown notification received, exiting");
                    break;
                }
            }
        }
    }

    Ok(())
}