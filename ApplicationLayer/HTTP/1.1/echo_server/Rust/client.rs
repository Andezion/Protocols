use hyper::{Client, Uri};

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error + Send + Sync>> {
    let uri: Uri = "http://httpbin.org/ip".parse()?;

    let client = Client::new();
    let res = client.get(uri).await?;

    println!("Status: {}", res.status());

    let body_bytes = hyper::body::to_bytes(res.into_body()).await?;
    println!("Body: {}", String::from_utf8_lossy(&body_bytes));

    Ok(())
}
