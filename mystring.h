#ifndef _MYSTRING_H_
#define _MYSTRING_H_

#include <iostream>
#include <cstring>

class MyString {
private:
    char* buffer;
    size_t length;
    size_t capacity;

    void growBuffer() {
        capacity *= 2; // 双倍增长策略
        char* newBuffer = new char[capacity];
        std::strcpy(newBuffer, buffer);
        delete[] buffer;
        buffer = newBuffer;
    }

public:
    MyString() : length(0), capacity(16) {
        buffer = new char[capacity];
        buffer[0] = '\0';
    }

    MyString(const char* str) : length(std::strlen(str)), capacity(length * 2) {
        buffer = new char[capacity];
        std::strcpy(buffer, str);
    }

    ~MyString() {
        delete[] buffer;
    }

    MyString& append(const char* str) {
        size_t newLength = length + std::strlen(str);
        while (newLength >= capacity) {
            growBuffer();
        }
        std::strcpy(buffer + length, str);
        length = newLength;
        return *this;
    }

    MyString& operator+=(const char* str) {
        return append(str);
    }

    const char* c_str() const {
        return buffer;
    }
};

#endif//_MYSTRING_H_