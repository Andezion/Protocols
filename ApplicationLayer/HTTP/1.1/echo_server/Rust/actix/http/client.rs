use std::process::Command;

fn main() -> std::io::Result<()> {
    let output = Command::new("curl")
        .arg("-i")
        .arg("-X")
        .arg("POST")
        .arg("http://localhost:8080/users")
        .arg("-H")
        .arg("Content-Type: application/json")
        .arg("-d")
        .arg(r#"{"name": "angmar"}"#)
        .output()?;

    println!("Status: {}", output.status);
    println!("Response: {}", String::from_utf8_lossy(&output.stdout));

    

    Ok(())
}