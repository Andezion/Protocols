use std::convert::Infallible;
use std::net::SocketAddr;

use http_body_util::Full;
use hyper::body::Bytes;
use hyper::server::conn::http1;
use hyper::service::service_fn;
use hyper::{Request, Response};
use hyper_util::rt::TokioIo;
use tokio::net::TcpListener;

async fn handle_request(_: Request<hyper::body::Incoming>) -> Result<Responce<Full<Bytes>>, Infallible> {
    Ok(Responce::new(Full::new(Bytes::from("Hello bithces"))))
} 