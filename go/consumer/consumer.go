package main

import (
	"encoding/gob"
	"fmt"
	"net"
	"time"

)

type TimeRange struct {
	StartTime, EndTime int64
}

type ServerData struct {
	Vec []int32
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
		endTime := time.Now().UnixNano() / 100
		startTime := (time.Now().UnixNano() / 100) - 1*1000000
		fmt.Printf("Requesting from %d to %d\n", startTime, endTime)

		err := encoder.Encode(TimeRange{startTime, endTime})
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
