use axum::{
    routing::{get, post},
    http::StatusCode,
    Json, Router,
}
use serde::{Deserialize, Serialize};

#[tokio::main]
async fn main() {
    tracing_subscriber::fmt::init();

    let app = Router::new()
        .route("/", get(root))
        .route("/users", post(users));

    let listener = tokio::net::TcpListener::bind("0:0:0:0:8080").await.unwrap();
    println!("Listening on http://{}", listener.local_addr().unwrap());

    axum::serve(listener, app).await.unwrap();
}

async fn root() -> &'static str {
    "Hello bithces"
}

async fn users(Json(payload) : Json<CreateUser>) -> (StatusCode, Json<User>) {
    let user = User {
        id: 1,  
        name: payload.name,
    }

    (StatusCode::CREATED, Json(user))
}