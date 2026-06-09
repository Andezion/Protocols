use std::env;
use std::io;
use std::net::UdpSocket;
use std::time::Duration;

fn resolve_name(socket: &UdpSocket, server_addr: &str, name: &str) -> io::Result<String> {
	let request = format!("QUERY {name}");
	socket.send_to(request.as_bytes(), server_addr)?;

	let mut buffer = [0_u8; 1024];
	let (received, _) = socket.recv_from(&mut buffer)?;
	let response = String::from_utf8_lossy(&buffer[..received]).trim().to_string();

	if let Some(ip) = response.strip_prefix("OK ") {
		Ok(ip.trim().to_string())
	} else {
		Err(io::Error::other(response))
	}
}

fn main() -> io::Result<()> {
	let mut args = env::args().skip(1);
	let server_addr = args
		.next()
		.unwrap_or_else(|| String::from("127.0.0.1:5300"));
	let names: Vec<String> = args.collect();

	let names = if names.is_empty() {
		vec![String::from("alice.local"), String::from("bob.local")]
	} else {
		names
	};

	let socket = UdpSocket::bind("127.0.0.1:0")?;
	socket.set_read_timeout(Some(Duration::from_secs(2)))?;

	for name in names {
		match resolve_name(&socket, &server_addr, &name) {
			Ok(ip) => println!("{name} -> {ip}"),
			Err(error) => eprintln!("{name} lookup failed: {error}"),
		}
	}

	Ok(())
}