#ifndef _SWIFTJSON_H_
#define _SWIFTJSON_H_

#include <sys/types.h>

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

class SwiftJson {
   private:
    static constexpr size_t defaultCapcity = 128;
    static constexpr char FIELD_START[] = "{";
    static constexpr char FIELD_END[] = "}";

   public:
    SwiftJson() : totalSize(0), needComma(false) {
        strStack.reserve(defaultCapcity);
        strSizeStack.reserve(defaultCapcity);
    }
    ~SwiftJson() = default;
    inline void clear() {
        strSizeStack.clear();
        strStack.clear();
    };
    SwiftJson &operator=(SwiftJson &&other) {
        if (this == &other) return *this;  // 防止自赋值
        totalSize = other.totalSize;
        needComma = other.needComma;
        strStack = std::move(other.strStack);
        strSizeStack = std::move(other.strSizeStack);
        other.totalSize = 0;  // 置源对象为有效但未定义的状态
        return *this;
    }
    inline void append(const char *cstr, int size) {
        strSizeStack.emplace_back(size);
        strStack.emplace_back(cstr);
        totalSize += size;
    }
    inline void append(const std::string &str) {
        append(str.data(), str.size());
    };
    template <std::size_t N>
    inline void append(const char (&cstr)[N]) {
        append(cstr, N - 1);
    }

   public:
    inline SwiftJson &operator+=(const std::string &str) {
        append(str);
        return *this;
    }
    template <std::size_t N>
    inline SwiftJson &operator+=(const char (&cstr)[N]) {
        append(cstr);
        return *this;
    }
    inline SwiftJson &&operator+(const std::string &str) {
        append(str);
        return std::move(*this);
    }
    template <std::size_t N>
    inline SwiftJson &&operator+(const char (&cstr)[N]) {
        append(cstr);
        return std::move(*this);
    }

   public:
    inline void start() { append(FIELD_START); }
    inline void end() { append(FIELD_END); }
    template <std::size_t N>
    inline void appendKey(const char (&cstr)[N]) {
        if (LIKELY(needComma)) {
            append(R"(, ")");
        } else {
            needComma = true;
            append(R"(")");
        }
        append(cstr);
        append(R"(":")");
    }
    inline void appendKey(const std::string &str) {
        if (LIKELY(needComma)) {
            append(R"(, ")");
        } else {
            needComma = false;
            append(R"(")");
        }
        append(str);
        append(R"(":")");
    }
    template <std::size_t N>
    inline void appendValue(const char (&cstr)[N]) {
        append(cstr);
        append(R"(")");
    }
    inline void appendValue(const std::string &str) {
        append(str);
        append(R"(")");
    }

    inline std::string toString() {
        std::string res;
        res.reserve(totalSize);
        int top = strStack.size();
        for (int i = 0; i < top; ++i) {
            res.append(strStack[i], strSizeStack[i]);
        }
        return res;
    }

   private:
    u_int64_t totalSize;
    bool needComma;
    std::vector<const char *> strStack;
    std::vector<u_int64_t> strSizeStack;
};

#endif  //_SWIFTJSON_H_