use actix_web::{get, post, web, App, HttpRequest, HttpServer, Responder};

#[get("/")]
async fn func_hello() -> impl Responder {
    HttpResponse::Ok().body("Hello, World!")
}