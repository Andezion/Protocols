use std::process::Command;

fn main() {
    let output = Command::new("curl")
        .arg("-X")
        .arg("POST")
        .arg("-H")
        .arg("Content-Type: application/json")
        .arg("-d")
        .arg(r#"{"name": "Penis"}"#)
        .arg("http://localhost:8080/users")
        .output()
        .expect("Failed to execute curl command");

    println!("Status: {}", output.status);
    println!("Response: {}", String::from_utf8_lossy(&output.stdout));
}