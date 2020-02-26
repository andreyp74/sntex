package main

import (
	"encoding/gob"
	"fmt"
	"net"
	"os"

)

type SampleRange struct {
	Count int64
}

type ServerData struct {
	Vec []int
}

func handleConnection(conn net.Conn, storage Storage) {
	remoteAddr := conn.RemoteAddr().String()
	fmt.Printf("Client connected from %s\n", remoteAddr)

	encoder := gob.NewEncoder(conn)
	decoder := gob.NewDecoder(conn)

	defer conn.Close()

	for {

		var sampleRange SampleRange
		err := decoder.Decode(&sampleRange)
		if err != nil {
			fmt.Printf("Receive error: %s\n", err)
			break
		}
		fmt.Printf("Requested %d samples of data\n", sampleRange.Count)

		val, ok := storage.Get(5)
		if !ok {
			fmt.Println("Couldn't retrieve value from storage")
			break
		}
		err = encoder.Encode(ServerData{val})
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

	storage := NewStorage()

	for {
		conn, err := server.Accept()
		if err != nil {
			fmt.Printf("tcp server accept error: %s\n", err)
			return
		}
		go handleConnection(conn, storage)
	}
}
