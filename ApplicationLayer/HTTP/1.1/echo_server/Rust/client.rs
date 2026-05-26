use hyper::Request;
use hyper::Body;
use hyper_util::rt::TokioIo;
use tokio::net::TcpStream;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error + Send + Sync>> {
    let url = "http://httpbin.org/ip".parse::<hyper::Uri>()?;

    let host = url.host().expect("uri has no host");
    let port = url.port_u16().unwrap_or(80);

    let address = format!("{}:{}", host, port);

    let stream = TcpStream::connect(address).await?;

    let io = TokioIo::new(stream);

    let (mut sender, conn) = hyper::client::conn::http1::handshake(io).await?;

    tokio::task::spawn(async move {
        if let Err(err) = conn.await {
            println!("Connection failed: {:?}", err);
        }
    });

    let req = Request::builder()
        .method("GET")
        .uri(url)
        .header("Host", host)
        .body(Body::empty())?;

    let res = sender.send_request(req).await?;
    println!("Response: {}", res.status());

    let body_bytes = hyper::body::to_bytes(res.into_body()).await?;
    println!("Body: {}", String::from_utf8_lossy(&body_bytes));

    Ok(())
}
