use rust_dns::{DnsServer, DnsRecord};

fn main() -> std::io::Result<()> {
    let server = DnsServer::new("127.0.0.1:5300")?;

    server.add_record(DnsRecord::new("example.com", "127.0.0.1"));
    server.add_record(DnsRecord::new("zalupa.com", "127.0.0.1"));
    
    server.run()
}