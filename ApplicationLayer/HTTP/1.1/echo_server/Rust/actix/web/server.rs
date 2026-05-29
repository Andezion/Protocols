use actix_web::{get, post, web, App, HttpRequest, HttpServer, Responder};

#[get("/")]
async fn func_hello() -> impl Responder {
    HttpResponse::Ok().body("Hello, World!")
}

#[post("/echo")]
async fn func_echo(req: String) -> impl Responder {
    HttpResponse::Ok().body(req)
}

async fn manual_hello() -> impl Responder {
    HttpResponse::Ok().body("Hello, World!")
}

