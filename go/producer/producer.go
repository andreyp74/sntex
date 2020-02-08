package main

import (
	"encoding/gob"
	"fmt"
	"net"
	"os"

)

type TimeRange struct {
	StartTime, EndTime int64
}

type ServerData struct {
	Vec []int32
}

func handleConnection(conn net.Conn) {
	remoteAddr := conn.RemoteAddr().String()
	fmt.Printf("Client connected from %s\n", remoteAddr)

	encoder := gob.NewEncoder(conn)
	decoder := gob.NewDecoder(conn)

	defer conn.Close()

	for {

		var timeRange TimeRange
		err := decoder.Decode(&timeRange)
		if err != nil {
			fmt.Printf("Receive error: %s\n", err)
			break
		}
		fmt.Printf("Requested data from: %d to %d\n", timeRange.StartTime, timeRange.EndTime)

		//TODO get value from storage
		tmp := []int32{1234, 190575, 1974, 17080302}
		//
		err = encoder.Encode(ServerData{tmp})
		if err != nil {
			fmt.Println("Send error: ", err)
			break
		}
	}
	fmt.Println("Connection closed")
}

func main() {

	arguments := os.Args
	PORT := ":9191"
	if len(arguments) > 1 {
		PORT = ":" + arguments[1]
	}

	server, err := net.Listen("tcp", PORT)
	if err != nil {
		fmt.Printf("tcp server listener error: %s\n", err)
		return
	}
	defer server.Close()

	fmt.Printf("Server strated at %s\n", PORT)

	for {
		conn, err := server.Accept()
		if err != nil {
			fmt.Printf("tcp server accept error: %s\n", err)
			return
		}
		go handleConnection(conn)
	}
}
