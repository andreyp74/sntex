package main

import (
	"encoding/gob"
	"fmt"
	"net"
	"time"

)

type SampleRange struct {
	Count int64
}

type ServerData struct {
	Vec []int
}

func main() {
	conn, err := net.Dial("tcp", "localhost:9191")
	if err != nil {
		fmt.Println("Couldn't connect to the server due to: ", err)
		return
	}
	encoder := gob.NewEncoder(conn)
	decoder := gob.NewDecoder(conn)

	defer conn.Close()

	fmt.Println("Client connected:", conn.RemoteAddr().String())

	for {
		var nSamples int64
		nSamples = 5
		fmt.Printf("Requesting %d samples of data\n", nSamples)

		err := encoder.Encode(SampleRange{Count: nSamples})
		if err != nil {
			fmt.Println("Send error: ", err)
			break
		}

		var msg ServerData
		err = decoder.Decode(&msg)
		if err != nil {
			fmt.Println("Receive error: ", err)
			break
		} else {
			fmt.Printf("Received %d items: %v\n", len(msg.Vec), msg.Vec)
		}

		time.Sleep(100 * time.Millisecond)
	}
	fmt.Println("Connection closed")
}
