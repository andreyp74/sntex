package main

import (
	"fmt"
	"sync"

)

type Storage struct {
	ch    chan int
	items []int
	mtx   sync.RWMutex
}

func NewStorage() Storage {
	ch := make(chan int)
	storage := Storage{ch: ch}
	go storage.run()

	return storage
}

func (storage Storage) run() {
	_ = NewProducer(storage.ch)
	for {
		sam := <-storage.ch
		fmt.Printf("Receive value %v\n", sam)
		storage.Put(sam)
	}
}

func (storage Storage) Put(value int) {
	storage.mtx.Lock()
	storage.items = append(storage.items, value)
	storage.mtx.Unlock()
}

func (storage Storage) Get(nSamples int64) ([]int, bool) {
	storage.mtx.RLock()
	sample := storage.items[:nSamples]
	storage.mtx.RUnlock()
	return sample, true
}
