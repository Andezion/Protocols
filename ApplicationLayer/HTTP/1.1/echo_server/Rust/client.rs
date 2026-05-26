use tokio::io::{AsyncReadExt, AsyncWriteExt};
use tokio::net::TcpStream;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error + Send + Sync>> {
    let host = "httpbin.org";
    let port = 80;
    let addr = format!("{}:{}", host, port);

    let mut stream = TcpStream::connect(addr).await?;

    let req = format!(
        "GET /ip HTTP/1.1\r\nHost: {}\r\nConnection: close\r\n\r\n",
        host
    );

    stream.write_all(req.as_bytes()).await?;

    let mut resp = Vec::new();
    stream.read_to_end(&mut resp).await?;

    println!("Response:\n{}", String::from_utf8_lossy(&resp));

    Ok(())
}
