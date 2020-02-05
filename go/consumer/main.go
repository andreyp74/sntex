package main

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"net"
	"time"
)

func send_time_period(conn net.Conn, start_time int64, end_time int64) error {
	buf := new(bytes.Buffer)
	if err := binary.Write(buf, binary.LittleEndian, start_time); err != nil {
		return err
	}
	if err := binary.Write(buf, binary.LittleEndian, end_time); err != nil {
		return err
	}
	if _, err := conn.Write(buf.Bytes()); err != nil {
		return err
	}
	return nil
}

func recv_message_size(conn net.Conn) (uint32, error) {
	var size uint32
	buf_size := make([]byte, 4)
	if _, err := conn.Read(buf_size); err != nil {
		return size, err
	}
	r := bytes.NewReader(buf_size)
	if err := binary.Read(r, binary.LittleEndian, &size); err != nil {
		return size, err
	}
	return size, nil
}

func recv_message(conn net.Conn, size uint32) ([]int32, error) {
	buf_data := make([]byte, size)
	if _, err := conn.Read(buf_data); err != nil {
		return nil, err
	}
	msg := make([]int32, size/4)
	r := bytes.NewReader(buf_data)
	if err := binary.Read(r, binary.LittleEndian, &msg); err != nil {
		return nil, err
	}
	return msg, nil
}

func recv_data(conn net.Conn) ([]int32, error) {
	size, err := recv_message_size(conn)
	if err != nil {
		return nil, err
	}
	msg, err := recv_message(conn, size)
	if err != nil {
		return nil, err
	}
	return msg, nil
}

func main() {
	conn, err := net.Dial("tcp", "localhost:9191")
	if err != nil {
		fmt.Println("Couldn't connect to the server due to: ", err)
		return
	}
	defer conn.Close()

	fmt.Println("Client connected:", conn.RemoteAddr().String())

	for {
		end_time := time.Now().UnixNano() / 100
		start_time := (time.Now().UnixNano() / 100) - 1*1000000
		fmt.Printf("Requesting from %d to %d\n", start_time, end_time)

		err := send_time_period(conn, start_time, end_time)
		if err != nil {
			fmt.Println("Send error: ", err)
			break
		}

		msg, err := recv_data(conn)
		if err != nil {
			fmt.Println("Receive error: ", err)
			break
		} else {
			fmt.Printf("Received %d items: %v\n", len(msg), msg)
		}

		time.Sleep(100 * time.Millisecond)
	}
	fmt.Println("Connection closed")
}
