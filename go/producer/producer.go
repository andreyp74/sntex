package main

import (
	"fmt"
	"time"

)

type Producer struct {
	done bool
	ch   chan int
}

func NewProducer(ch chan int) Producer {
	producer := Producer{done: false, ch: ch}
	go producer.run()

	return producer
}

func (prod Producer) Stop() {
	prod.done = true
}

func (prod Producer) run() {
	InitGenerator()

	for {
		if prod.done {
			break
		}

		sam := GetNext()
		prod.ch <- sam
		fmt.Printf("Sent sample value %v\n", sam)

		time.Sleep(time.Second)
	}
}
