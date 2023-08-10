#ifndef _STRINGAPPENDBUFFER_HPP_
#define _STRINGAPPENDBUFFER_HPP_

#include <cstddef>
#include <string_view>

#include "Buffer.hpp"

class StringAppendBuffer {
   public:
    StringAppendBuffer() : buffer(defaultCapacity) {}

    StringAppendBuffer(size_t capacity) : buffer(capacity) {}
    StringAppendBuffer(std::string_view sv) : buffer(defaultCapacity) {
        buffer.append(sv.data(), sv.size());
    }
    StringAppendBuffer(std::string_view sv, std::size_t capacity)
        : buffer(capacity) {
        buffer.append(sv.data(), sv.size());
    }

    ~StringAppendBuffer() = default;

    StringAppendBuffer(StringAppendBuffer&& other) noexcept
        : buffer(std::move(other.buffer)) {}

    inline std::string toString() {
        return std::string(buffer.begin(), buffer.end());
    }

    inline void clear() { buffer.clear(false); }

    inline std::size_t size() { return buffer.size(); }

    inline std::size_t capacity() { return buffer.capacity(); }

    inline bool empty() { return buffer.empty(); };

    inline void setCapacity(std::size_t capacity) {
        buffer.setCapacity(capacity, true);
    }

    inline void append(std::string_view sv) {
        buffer.append(sv.data(), sv.size());
    }

    inline StringAppendBuffer& operator<<(std::string_view sv) {
        append(sv);
        return *this;
    }

    inline StringAppendBuffer& operator<<(const StringAppendBuffer& other) {
        if (this != &other) {
            buffer.append(other.buffer);
        }
        return *this;
    }

    inline StringAppendBuffer& operator<<(StringAppendBuffer&& other) {
        if (this != &other) {
            buffer.append(other.buffer);
        }
        return *this;
    }

    inline StringAppendBuffer& operator=(const StringAppendBuffer& other) {
        if (this != &other) {
            buffer = other.buffer;  // 调用Buffer类的赋值运算符
        }
        return *this;
    }

    inline StringAppendBuffer& operator=(StringAppendBuffer&& other) {
        if (this != &other) {
            buffer = other.buffer;  // 调用Buffer类的赋值运算符
        }
        return *this;
    }

   private:
    constexpr static size_t defaultCapacity = 64;
    Fundation::Buffer<char> buffer;
};

#endif  //_STRINGAPPENDBUFFER_HPP_