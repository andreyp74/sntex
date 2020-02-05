package main

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"net"
	"os"
)

func recv_time_period(conn net.Conn) (int64, int64, error) {
	buf := make([]byte, 16)
	if _, err := conn.Read(buf); err != nil {
		return 0, 0, err
	}
	r := bytes.NewReader(buf)

	var start_time int64
	var end_time int64
	if err := binary.Read(r, binary.LittleEndian, &start_time); err != nil {
		return 0, 0, err
	}
	if err := binary.Read(r, binary.LittleEndian, &end_time); err != nil {
		return 0, 0, err
	}
	return start_time, end_time, nil
}

func send_message(conn net.Conn, data []int32) error {
	buf := new(bytes.Buffer)
	var size uint32 = uint32(len(data)) * 4
	if err := binary.Write(buf, binary.LittleEndian, size); err != nil {
		return err
	}
	for _, v := range data {
		if err := binary.Write(buf, binary.LittleEndian, v); err != nil {
			return err
		}
	}
	if _, err := conn.Write(buf.Bytes()); err != nil {
		return err
	}

	return nil
}

func handle_connection(conn net.Conn) {
	remoteAddr := conn.RemoteAddr().String()
	fmt.Printf("Client connected from %s\n", remoteAddr)
	defer conn.Close()

	for {
		start_time, end_time, err := recv_time_period(conn)
		if err != nil {
			fmt.Printf("Receive error: %s\n", err)
			break
		}

		fmt.Printf("Requested data from: %d to %d\n", start_time, end_time)

		//TODO get value from storage
		data := []int32{1234, 190575, 1974, 17080302}
		//
		if err := send_message(conn, data); err != nil {
			fmt.Printf("Send error: %s\n", err)
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
		go handle_connection(conn)
	}
}
