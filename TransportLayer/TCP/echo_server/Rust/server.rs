use std::{io, net::{TcpListener, TcpStream}, thread};

fn main() -> io::Result<()> {
    let listener = TcpListener::bind(":8090")?;

    println!("Echo server is running on :8090");

    

}

