use std::process::Command;

fn main() -> std::io::Result<()> {
    let output_empty = Command::new("curl")
    .arg("-i")
    .arg("http://127.0.0.1:8080/")
    .output()?;

    println!("Status: {}", output.status);
    println!("Response: {}", String::from_utf8_lossy(&output.stdout));

    let output_hey = Command::new("curl")
    .arg("-i")
    .arg("http://127.0.0.1:8080/hey")
    .output()?;

    println!("Status: {}", output.status);
    println!("Response: {}", String::from_utf8_lossy(&output.stdout));

    Ok(())
}