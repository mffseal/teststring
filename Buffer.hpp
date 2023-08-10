#ifndef _BUFFER_HPP_
#define _BUFFER_HPP_

#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <utility>

namespace Fundation {
template <class T>
class Buffer {
   public:
    Buffer(std::size_t length)
        : _capacity(length), _used(length), _ptr(0), _ownMem(true) {
        if (length > 0) _ptr = new T[length];
    }

    Buffer(T* pMem, std::size_t length)
        : _capacity(length), _used(length), _ptr(pMem), _ownMem(false) {}

    Buffer(const T* pMem, std::size_t length)
        : _capacity(length), _used(length), _ptr(pMem), _ownMem(true) {
        if (_capacity > 0) {
            _ptr = new T[_capacity];
            std::memcpy(_ptr, pMem, _used * sizeof(T));
        }
    }

    Buffer(Buffer&& other) noexcept
        : _capacity(other._capacity),
          _used(other._used),
          _ptr(other._ptr),
          _ownMem(other._ownMem) {
        other._capacity = 0;
        other._used = 0;
        other._ownMem = false;
        other._ptr = nullptr;
    }

    Buffer& operator=(const Buffer& other) {
        if (this != &other) {
            Buffer tmp(other);
            swap(tmp);
        }
        return *this;
    }

    Buffer& operator=(Buffer&& other) {
        if (_ownMem) delete[] _ptr;
        _capacity = other._capacity;
        _used = other._used;
        _ptr = other._ptr;
        _ownMem = other._ownMem;

        other._capacity = 0;
        other._used = 0;
        other._ownMem = false;
        other._ptr = nullptr;

        return *this;
    }

    ~Buffer() {
        if (_ownMem) delete[] _ptr;
    }

    void resize(std::size_t newCapacity, bool preserveContent = true) {
        if (!_ownMem) return;

        if (newCapacity > _capacity) {
            T* ptr = new T[newCapacity];
            if (preserveContent && _ptr) {
                std::memcpy(ptr, _ptr, _used * sizeof(T));
            }
            delete[] _ptr;
            _ptr = ptr;
            _capacity = newCapacity;
        }
    }

    void setCapacity(std::size_t newCapacity, bool preserveContent = true) {
        if (!_ownMem) return;

        if (newCapacity != _capacity) {
            T* ptr = 0;
            if (newCapacity > 0) {
                ptr = new T[newCapacity];
                if (preserveContent && _ptr) {
                    std::size_t newSz =
                        _used < newCapacity ? _used : newCapacity;
                    std::memcpy(ptr, _ptr, newSz * sizeof(T));
                }
            }
            delete[] _ptr;
            _ptr = ptr;
            _capacity = newCapacity;

            if (newCapacity < _used) _used = newCapacity;
        }
    }

    void assign(const T* buf, std::size_t sz) {
        if (0 == sz) return;
        if (sz > _capacity) resize(sz, false);
        std::memcpy(_ptr, buf, sz * sizeof(T));
        _used = sz;
    }

    void append(const T* buf, std::size_t sz) {
        if (0 == sz) return;
        resize(_used + sz, true);
        std::memcpy(_ptr + _used - sz, buf, sz * sizeof(T));
        _used += sz;
    }

    void append(T val) {
        resize(_used + 1, true);
        _ptr[_used - 1] = val;
    }

    void append(const Buffer& buf) { append(buf.begin(), buf.size()); }

    std::size_t capacity() const { return _capacity; }

    std::size_t capacityBytes() const { return _capacity * sizeof(T); }

    void swap(Buffer& other) {
        using std::swap;
        swap(_ptr, other._ptr);
        swap(_capacity, other.capacity);
        swap(_used, other._used);
        swap(_ownMem, other._ownMem);
    }

    bool operator==(const Buffer& other) const {
        if (this != &other) {
            if (_used == other._used) {
                if (_ptr && other._ptr &&
                    std::memcmp(_ptr, other._ptr, _used * sizeof(T)) == 0) {
                    return true;
                } else
                    return _used == 0;
            }
            return false;
        }
        return true;
    }

    bool operator!=(const Buffer& other) const { return !(*this == other); }

    void clear() { std::memset(_ptr, 0, _used * sizeof(T)); }

    std::size_t size() const { return _used; }

    std::size_t sizeBytes() const { return _used * sizeof(T); }

    T* begin() { return _ptr; }

    const T* begin() const { return _ptr; }

    T* end() { return _ptr + _used; }

    const T* end() const { return _ptr + _used; }

    bool empty() const { return 0 == _used; }

    T& operator[](std::size_t index) {
        if (index >= _used) throw std::out_of_range("Index out of range");
        return _ptr[index];
    }

    const T& operator[](std::size_t index) const {
        if (index >= _used) throw std::out_of_range("Index out of range");
        return _ptr[index];
    }

   private:
    Buffer();
    std::size_t _capacity;
    std::size_t _used;
    T* _ptr;
    bool _ownMem;
};
}  // namespace Fundation

#endif  //_BUFFER_HPP_