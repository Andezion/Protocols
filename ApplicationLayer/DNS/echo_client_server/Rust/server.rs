use rust_dns::{DnsServer, DnsRecord};
fn main() -> std::io::Result<()> {
    // Create a new DNS server instance
    let server = DnsServer::new("127.0.0.1:5300")?;
    server.add_record(DnsRecord::new("example.com", "192.168.1.1"));
    
    // Run the server
    server.run()
}