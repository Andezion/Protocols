use std::process::Command;

fn main() -> std::io::Result<()> {
    let output_empty = Command::new("curl")
    .arg("-i")
    .arg("http://127.0.0.1:8080/")
    .output()?;

    println!("Status: {}", output_empty.status);
    println!("Response: {}", String::from_utf8_lossy(&output_empty.stdout));

    let output_hey = Command::new("curl")
    .arg("-i")
    .arg("http://127.0.0.1:8080/hey")
    .output()?;

    println!("Status: {}", output_hey.status);
    println!("Response: {}", String::from_utf8_lossy(&output_hey.stdout));

    let output_echo = Command::new("curl")
    .arg("-i")
    .arg("-X")
    .arg("POST")
    .arg("http://127.0.0.1:8080/echo")
    .arg("-H")
    .arg("Content-Type: text/plain")
    .arg("-d")
    .arg("zalupa")
    .output()?;

    println!("Status: {}", output_echo.status);
    println!("Response: {}", String::from_utf8_lossy(&output_echo.stdout));

    Ok(())
}