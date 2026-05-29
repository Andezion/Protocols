use actix_web::{get, post, web, App, HttpRequest, HttpServer, Responder, HttpResponse};

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

#[actix_web::main]
async fn main() -> std::io::Result<()> {
    HttpServer::new(|| {
        App::new()
            .service(func_hello)
            .service(func_echo)
            .route("/hey", web::get().to(manual_hello))
    })
    .bind(("127.0.0.1", 8080))?
    .run()
    .await
}