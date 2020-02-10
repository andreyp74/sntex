package storage

import (
	"sync"
)

type Storage struct {
	items map[int64]int32
	mtx   sync.RWMutex
}

func New() Storage {
	return Storage{items: make(map[int64]int32)}
}

func (st Storage) Put(key int64, value int32) {
	st.mtx.Lock()
	st.items[key] = value
	st.mtx.Unlock()
}

func (st Storage) Get(key int64) ([]int32, bool) {
	st.mtx.RLock()
	value, ok := st.items[key]
	st.mtx.RUnlock()
	return []int32{value}, ok
}
