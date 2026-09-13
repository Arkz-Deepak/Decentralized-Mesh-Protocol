#pragma once
#include <cstddef>
#include<mutex>

template <typename T, size_t MAX_QUEUE_SIZE>
class CircularBuffer{
    private:
    mutable std::mutex mtx;
    T queue[MAX_QUEUE_SIZE];
    size_t h_index;
    size_t t_index;
    size_t count;

    public:
    CircularBuffer();
    bool enqueue(const T& item);
    bool dequeue(T& item);
    bool is_full() const;
    bool is_empty() const;
    bool contains(const T& item) const;
    void clear();
    size_t size() const;
};
template <typename T, size_t MAX_QUEUE_SIZE>
CircularBuffer<T, MAX_QUEUE_SIZE>::CircularBuffer() {
    h_index = 0;
    t_index = 0;
    count = 0;
}

template <typename T, size_t MAX_QUEUE_SIZE>
bool CircularBuffer<T, MAX_QUEUE_SIZE>::is_empty() const {
    return count == 0;
}

template <typename T, size_t MAX_QUEUE_SIZE>
bool CircularBuffer<T, MAX_QUEUE_SIZE>::is_full() const {
    return count == MAX_QUEUE_SIZE;
}

template <typename T, size_t MAX_QUEUE_SIZE>
bool CircularBuffer<T, MAX_QUEUE_SIZE>::enqueue(const T& item) {
    std::lock_guard<std::mutex> lock(mtx);

    if (count == MAX_QUEUE_SIZE) {
        return false;
    }

    queue[t_index] = item;

    t_index = (t_index + 1) % MAX_QUEUE_SIZE;

    count += 1;
    
    return true;
}

template <typename T, size_t MAX_QUEUE_SIZE>
bool CircularBuffer<T, MAX_QUEUE_SIZE>::dequeue(T& item) {
    std::lock_guard<std::mutex> lock(mtx);

    if (count == 0) {
        return false;
    }

    item = queue[h_index];

    h_index = (h_index + 1) % MAX_QUEUE_SIZE;
    
    count -= 1;
    
    return true;
}

template <typename T, size_t MAX_QUEUE_SIZE>
bool CircularBuffer<T, MAX_QUEUE_SIZE>::contains(const T& item) const {
    std::lock_guard<std::mutex> lock(mtx);

    for (size_t i = 0; i < count; ++i) {
        size_t idx = (h_index + i) % MAX_QUEUE_SIZE;
        if (queue[idx] == item) {
            return true;
        }
    }
    return false;
}

template <typename T, size_t MAX_QUEUE_SIZE>
void CircularBuffer<T, MAX_QUEUE_SIZE>::clear() {
    std::lock_guard<std::mutex> lock(mtx);
    h_index = 0;
    t_index = 0;
    count = 0;
}

template <typename T, size_t MAX_QUEUE_SIZE>
size_t CircularBuffer<T, MAX_QUEUE_SIZE>::size() const {
    std::lock_guard<std::mutex> lock(mtx);
    return count;
}