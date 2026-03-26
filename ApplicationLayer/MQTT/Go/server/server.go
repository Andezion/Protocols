package main

import (
	"fmt"
	"math/rand"
	"time"

	mqtt "github.com/eclipse/paho.mqtt.golang"
)

const (
	broker   = "tcp://localhost:1883"
	clientID = "go-mqtt-publisher"
	topic    = "iot-messages"
)

var messages = []string{
	"Hello, World!",
	"Greetings from Go!",
	"MQTT is awesome!",
	"Random message incoming!",
	"Go is fun!",
}

func main() {
	opts := mqtt.NewClientOptions()
	opts.AddBroker(broker)
	opts.SetClientID(clientID)
	opts.OnConnect = func(c mqtt.Client) { fmt.Println("Connected to broker") }
	opts.OnConnectionLost = func(c mqtt.Client, err error) { fmt.Printf("Connection lost: %v\n", err) }

	client := mqtt.NewClient(opts)
	if token := client.Connect(); token.Wait() && token.Error() != nil {
		panic(token.Error())
	}

	for {
		msg := messages[rand.Intn(len(messages))]
		client.Publish(topic, 0, false, msg).Wait()
		fmt.Printf("Published: %s\n", msg)
		time.Sleep(time.Second)
	}
}
