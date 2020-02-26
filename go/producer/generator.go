package main

import (
	"math/rand"
	"time"

)

func InitGenerator() {
	rand.Seed(time.Now().UnixNano())
}

func GetNext() int {
	return rand.Int()
}
