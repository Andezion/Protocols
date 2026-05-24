package main

import (
	"encoding/json"
	"fmt"
	"log"
	"net/http"
)

func pingHandler(w http.ResponseWriter, r *http.Request) {
	log.Printf("%s %s", r.Method, r.URL.Path)
	response := map[string]string{"message": "pong"}
	w.Header().Set("Content-Type", "application/json")

	if err := json.NewEncoder(w).Encode(response); err != nil {
		http.Error(w, "failed to encode response", http.StatusInternalServerError)
		log.Printf("pingHandler: encode error: %v", err)
		return
	}
}

func handler(w http.ResponseWriter, r *http.Request) {
	log.Printf("%s %s", r.Method, r.URL.Path)
	fmt.Fprintf(w, "vlad topchik")
}

func echoHandler(w http.ResponseWriter, r *http.Request) {
	log.Printf("%s %s", r.Method, r.URL.Path)

	body := make([]byte, r.ContentLength)
	r.Body.Read(body)

	w.Header().Set("Content-Type", "application/octet-stream")
	w.Write(body)
}

func custom_handler(w http.ResponseWriter, r *http.Request) {
	log.Printf("%s %s", r.Method, r.URL.Path)
	fmt.Fprintf(w, "u r gay lol")
}

func custom_request_handler(w http.ResponseWriter, r *http.Request) {
	log.Printf("%s %s", r.Method, r.URL.Path)

	if r.Method == http.MethodGet {
		w.Header().Set("Content-Type", "text/html; charset=utf-8")
		fmt.Fprintf(w, `
					<html>
					<body>
						<form method="POST">
							<input type="text" name="name" placeholder="Enter your name">
							<button type="submit">Submit</button>
						</form>
					</body>
					</html>
        `)
	} else if r.Method == http.MethodPost {
		err := r.ParseForm()
		if err != nil {
			http.Error(w, "failed to parse form", http.StatusBadRequest)
			log.Printf("custom_request_handler: parse form error: %v", err)
			return
		}

		name := r.FormValue("name")
		w.Header().Set("Content-Type", "text/html; charset=utf-8")
		fmt.Fprintf(w, "Hello, %s!", name)
	}
}

func main() {
	http.HandleFunc("/", handler)
	http.HandleFunc("/ping", pingHandler)
	http.HandleFunc("/echo", echoHandler)
	http.HandleFunc("/custom", custom_handler)
	http.HandleFunc("/form", custom_request_handler)

	log.Println("Starting server on localhost:8080")
	if err := http.ListenAndServe("localhost:8080", nil); err != nil {
		log.Fatalf("ListenAndServe: %v", err)
	}
}
