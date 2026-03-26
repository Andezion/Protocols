package main

import (
	"fmt"
	"os"
	"os/signal"
	"syscall"

	mqtt "github.com/eclipse/paho.mqtt.golang"
)

const (
	broker   = "tcp://localhost:1883"
	clientID = "go-mqtt-subscriber"
	topic    = "iot-messages"
)

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

	client.Subscribe(topic, 1, func(_ mqtt.Client, msg mqtt.Message) {
		fmt.Printf("Received [%s]: %s\n", msg.Topic(), msg.Payload())
	}).Wait()
	fmt.Printf("Subscribed to: %s\n", topic)

	sig := make(chan os.Signal, 1)
	signal.Notify(sig, os.Interrupt, syscall.SIGTERM)
	<-sig

	client.Unsubscribe(topic)
	client.Disconnect(250)
	fmt.Println("Disconnected")
}
