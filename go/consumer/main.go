package main

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"net"
	"time"
	"unsafe"

)

func send_time_period(conn net.Conn, start_time int64, end_time int64) error {
	st_buf := new(bytes.Buffer)
	err := binary.Write(st_buf, binary.BigEndian, start_time)
	if err != nil {
		return err
	}
	et_buf := new(bytes.Buffer)
	err = binary.Write(et_buf, binary.BigEndian, end_time)
	if err != nil {
		return err
	}
	_, err = conn.Write(st_buf.Bytes())
	if err != nil {
		return err
	}
	_, err = conn.Write(et_buf.Bytes())
	return err
}

func recv_data(conn net.Conn) (string, error) {
	var size int64
	bufSize := make([]byte, unsafe.Sizeof(size))
	_, err := conn.Read(bufSize)
	if err != nil {
		return "", err
	}

	size, err = binary.ReadVarint(bufSize)
	if err != nil {
		return "", err
	}

	bufData := make([]byte, size)
	_, err = conn.Read(bufData)
	if err != nil {
		return "", err
	}

	return string(bufData), err
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
		start_time := time.Now().UnixNano() / int64(time.Millisecond)
		end_time := (time.Now().UnixNano() / int64(time.Millisecond)) - 1*1000000
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
