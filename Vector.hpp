#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__
#define _CRT_SECURE_NO_WARNINGS
template <typename T>
class Vector {
    T* p_data = nullptr;
    size_t u32size = 0U;
    size_t u32capacity = 0U;

    void free() {
        delete[] p_data;
        p_data = nullptr;
        u32size = 0U;
        u32capacity = 0U;
    }

    void copyFrom(const Vector& p_other) {
        u32capacity = p_other.u32capacity;
        u32size = p_other.u32size;
        p_data = new T[u32capacity];
        for (size_t u32_idx = 0U; u32_idx < u32size; ++u32_idx) {
            p_data[u32_idx] = p_other.p_data[u32_idx];
        }
    }

    void resize(size_t u32newCapacity) {
        T* p_new_data = new T[u32newCapacity];
        for (size_t u32_idx = 0U; u32_idx < u32size; ++u32_idx) {
            p_new_data[u32_idx] = p_data[u32_idx];
        }
        delete[] p_data;
        p_data = p_new_data;
        u32capacity = u32newCapacity;
    }

public:
    Vector() : u32capacity(2U) {
        p_data = new T[u32capacity];
    }

    Vector(size_t u32initialCapacity) : u32capacity(u32initialCapacity) {
        p_data = new T[u32capacity];
    }

    Vector(const Vector& p_other) {
        copyFrom(p_other);
    }

    Vector& operator=(const Vector& p_other) {
        if (this != &p_other) {
            free();
            copyFrom(p_other);
        }
        return *this;
    }

    ~Vector() {
        free();
    }

    void push_back(const T& p_element) {
        if (u32size >= u32capacity) {
            resize((u32capacity == 0U) ? 2U : (u32capacity * 2U));
        }
        p_data[u32size++] = p_element;
    }

    void pop_back() {
        if (u32size > 0U) {
            u32size--;
        }
    }

    void clear() {
        u32size = 0U;
    }

    T& operator[](size_t u32index) {
        return p_data[u32index];
    }

    const T& operator[](size_t u32index) const {
        return p_data[u32index];
    }

    size_t getSize() const {
        return u32size;
    }

    bool isEmpty() const {
        return u32size == 0U;
    }
};
#endif // !__VECTOR_HPP__