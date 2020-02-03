package main

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"net"
	"time"
	"unsafe"
)

type Request struct {
	start_time int64
	end_time   int64
}

func send_time_period(conn net.Conn, start_time int64, end_time int64) error {
	buf := new(bytes.Buffer)
	err := binary.Write(buf, binary.LittleEndian, start_time)
	if err != nil {
		return err
	}
	err = binary.Write(buf, binary.LittleEndian, end_time)
	if err != nil {
		return err
	}
	_, err = conn.Write(buf.Bytes())
	if err != nil {
		return err
	}
	return nil
}

func recv_data(conn net.Conn) (string, error) {

	var size int64
	buf_size := make([]byte, unsafe.Sizeof(size))
	r := bytes.NewReader(buf_size)

	_, err := conn.Read(buf_size)
	if err != nil {
		return "", err
	}

	err = binary.Read(r, binary.LittleEndian, &size)
	if err != nil {
		return "", err
	}

	buf_data := make([]byte, size)
	r = bytes.NewReader(buf_data)
	_, err = conn.Read(buf_data)
	if err != nil {
		return "", err
	}

	return string(buf_data), nil
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
		}

		msg, err := recv_data(conn)
		if err != nil {
			fmt.Println("Receive error: ", err)
		} else {
			fmt.Println("Received ", msg)
		}

		time.Sleep(100 * time.Millisecond)
	}
}
