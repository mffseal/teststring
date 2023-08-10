#include <fmt/format.h>
#include <llvm-14/llvm/ADT/Twine.h>
#include <pthread.h>

#include <algorithm>
#include <chrono>
#include <ext/rope>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "SwiftJson.h"
#include "fast_string_concatenator.hpp"
#include "mystring.h"
#include "sonic/dom/generic_document.h"
#include "sonic/sonic.h"
#include "table_str.hh"
#include "Buffer.hpp"

#define THREAD_COUNT 1
#define REPEAT_TIMES 10

template <typename... Args>
constexpr auto concat(Args... args) {
    return (... + args);
}

// Helper function
void append_to_string_impl(std::string &str) {}

template <typename T, typename... Args>
void append_to_string_impl(std::string &str, const T &first, Args... args) {
    str += first;
    append_to_string_impl(str, args...);
}

// Macro
#define ADD_TO_STRING(str, ...) append_to_string_impl(str, __VA_ARGS__)

void appendToString(std::string &base) {}

template <typename T, typename... Args>
void appendToString(std::string &base, const T &str, Args... args) {
    base.append(str);
    appendToString(base, args...);  // recursive call using remaining arguments
}

typedef void *(*func_t)(void *);

struct TestFunction {
    std::string name;
    double overhead{};
    func_t func;

    TestFunction(std::string n, func_t f) : name(std::move(n)), func(f) {}
};

std::string str1 = "123123123123123123123123123123";
std::string str2 =
    "456456456456456456456456456456456456456456456456456456456456456";
std::string str3 =
    "78978978978978978978978978978978978978978978978978978978978"
    "9789789789789789789789789789789789789";

#define TSTR1 "abcdefghijk"
#define TSTR2 "ABCDEFGHIJK"
#define TSTR3 "!@#$%^&*()"

void *test_stringstream(void *args) {
    std::string s;
    std::stringstream ss;
    for (int i = 0; i < REPEAT_TIMES; i++) {
        ss << TSTR1 << str1 << TSTR2 << str2 << TSTR3 << str3;
    }
    s = ss.str();
    return nullptr;
}

void *test_snprintf(void *args) {
    std::string s;
    char buffer[4096];
    for (int i = 0; i < REPEAT_TIMES; i++) {
        snprintf(buffer, sizeof(buffer), "%s%s%s%s%s%s\n", TSTR1, str1.c_str(),
                 TSTR2, str2.c_str(), TSTR3, str3.c_str());
    }
    s = buffer;
    return nullptr;
}

void *test_append(void *args) {
    std::string s;
    // s.reserve((str1.length() + str2.length() + str3.length() + sizeof(TSTR1)
    // +
    //            sizeof(TSTR2) + sizeof(TSTR3)) *
    //           REPEAT_TIMES);
    for (int i = 0; i < REPEAT_TIMES; i++) {
        s.append(TSTR1);
        s.append(str1);
        s.append(TSTR2);
        s.append(str2);
        s.append(TSTR3);
        s.append(str3);
    }
    return nullptr;
}

void *test_fmt(void *args) {
    std::string s;
    // s.reserve((str1.length() + str2.length() + str3.length() + sizeof(TSTR1)
    // +
    //            sizeof(TSTR2) + sizeof(TSTR3)) *
    //           REPEAT_TIMES);
    for (int i = 0; i < REPEAT_TIMES; i++) {
        s = fmt::format("{}{}{}{}{}{}\n", TSTR1, str1, TSTR2, str2, "hig",
                        str3);
    }
    return nullptr;
}

void *test_fmt_buf(void *args) {
    fmt::memory_buffer buf;
    buf.reserve((str1.length() + str2.length() + str3.length() + sizeof(TSTR1) +
                 sizeof(TSTR2) + sizeof(TSTR3)) *
                REPEAT_TIMES);
    for (int i = 0; i < REPEAT_TIMES; i++) {
        fmt::format_to(std::back_inserter(buf), "{}{}{}{}{}{}\n", TSTR1, str1,
                       TSTR2, str2, "hig", str3);
    }
    fmt::to_string(buf);
    return nullptr;
}

void *test_add(void *args) {
    std::string s;
    // s.reserve((str1.length() + str2.length() + str3.length() + sizeof(TSTR1)
    // +
    //            sizeof(TSTR2) + sizeof(TSTR3)) *
    //           REPEAT_TIMES);
    for (int i = 0; i < REPEAT_TIMES; i++) {
        s += TSTR1;
        s += str1;
        s += TSTR2;
        s += str2;
        s += TSTR3;
        s += str3;
    }
    return nullptr;
}

void *test_add_obo(void *args) {
    std::string s;
    for (int i = 0; i < REPEAT_TIMES; i++) {
        ADD_TO_STRING(s, TSTR1, str1, TSTR2, str2, TSTR2, str3);
    }
    return nullptr;
}

void *test_append_obo(void *args) {
    std::string s;
    for (int i = 0; i < REPEAT_TIMES; i++) {
        appendToString(s, TSTR1, str1, TSTR2, str2, TSTR3, str3);
    }
    return nullptr;
}

void *test_append_short(void *args) {
    std::string s;
    // s.reserve((str1.length() + str2.length() + str3.length() + sizeof(TSTR1)
    // +
    //            sizeof(TSTR2) + sizeof(TSTR3)) *
    //           REPEAT_TIMES);
    for (int i = 0; i < REPEAT_TIMES; i++) {
        s.append("abcde");
        s.append("fgh");
        s.append("ijk");
        s.append(str1);
        s.append("AB");
        s.append("CDEF");
        s.append("GHI");
        s.append("JK");
        s.append(str2);
        s.append(TSTR3);
        s.append(str3);
    }
    return nullptr;
}

void *test_add_short(void *args) {
    std::string s;
    // s.reserve((str1.length() + str2.length() + str3.length() + sizeof(TSTR1)
    // +
    //            sizeof(TSTR2) + sizeof(TSTR3)) *
    //           REPEAT_TIMES);
    for (int i = 0; i < REPEAT_TIMES; i++) {
        s += "abcde";
        s += "fgh";
        s += "ijk";
        s += str1;
        s += "AB";
        s += "CDEF";
        s += "GHI";
        s += "JK";
        s += str2;
        s += TSTR3;
        s += str3;
    }
    return nullptr;
}

void *test_ternary_indirect(void *args) {
    std::string s;
    for (int i = 0; i < REPEAT_TIMES; i++) {
        int a = i % 2;
        std::string tmp = a == 0 ? "0" : "1";
        s += tmp;
    }
    return nullptr;
}

void *test_ternary_direct(void *args) {
    std::string s;
    for (int i = 0; i < REPEAT_TIMES; i++) {
        int a = i % 2;
        s += a == 0 ? "0" : "1";
    }
    return nullptr;
}

char cs[16] = "123456";
int csLen = 6;

void *test_assign(void *args) {
    std::string s;
    for (int i = 0; i < REPEAT_TIMES; i++) {
        std::string tmp;
        tmp.assign(cs, csLen);
        s += tmp;
    }
    return nullptr;
}

void *test_noappend(void *args) {
    std::string s;
    for (int i = 0; i < REPEAT_TIMES; i++) {
        s.append(cs, csLen);
    }
    return nullptr;
}

void *test_sonic(void *args) {
    sonic_json::Document doc;
    sonic_json::WriteBuffer wb;
    // wb.Reserve((str1.length() + str2.length() + str3.length() + sizeof(TSTR1)
    // +
    //             sizeof(TSTR2) + sizeof(TSTR3)) *
    //            REPEAT_TIMES);
    for (int i = 0; i < REPEAT_TIMES; i++) {
        wb.Push(TSTR1, sizeof(TSTR1) - 1);
        wb.Push(str1.data(), str1.size());
        wb.Push(TSTR2, sizeof(TSTR2) - 1);
        wb.Push(str2.data(), str2.size());
        wb.Push(TSTR3, sizeof(TSTR3) - 1);
        wb.Push(str3.data(), str3.size());
    }
    std::string s = wb.ToString();
    return nullptr;
}

void *test_sonic_obo(void *args) {
    sonic_json::Document doc;
    sonic_json::WriteBuffer wb;
    // wb.Reserve((str1.length() + str2.length() + str3.length() + sizeof(TSTR1)
    // +
    //             sizeof(TSTR2) + sizeof(TSTR3)) *
    //            REPEAT_TIMES);
    for (int i = 0; i < REPEAT_TIMES; i++) {
        wb.Push(str1.data(), str1.size());
        wb.Push("abcde", sizeof("abcde") - 1);
        wb.Push("fgh", sizeof("fgh") - 1);
        wb.Push("ijk", sizeof("ijk") - 1);
        wb.Push(str2.data(), str2.size());
        wb.Push("AB", sizeof("AB") - 1);
        wb.Push("CDEF", sizeof("CDEF") - 1);
        wb.Push("GHI", sizeof("GHI") - 1);
        wb.Push("JK", sizeof("JK") - 1);
        wb.Push(str3.data(), str3.size());
        wb.Push(TSTR3, sizeof(TSTR3) - 1);
    }
    std::string s = wb.ToString();
    return nullptr;
}

void *test_concat(void *args) {
    std::string s;
    for (int i = 0; i < REPEAT_TIMES; i++) {
        std::string hello = "Hello, ";
        std::string world = "World!";
        std::string how = " How ";
        std::string are = "are ";
        std::string you = "you?";
        s = concat(hello, world, how, are, you);
    }
    return nullptr;
}

void *test_noconcat(void *args) {
    const char *s;
    for (int i = 0; i < REPEAT_TIMES; i++) {
        s = "Hello, "
            "World!"
            " How "
            "are "
            "you?";
    }
    return nullptr;
}

void *test_fast_concat(void *args) {
    stlsoft::fsc_seed tmp_fsc;
    std::string s;
    // s.reserve((str1.length() + str2.length() + str3.length() + sizeof(TSTR1)
    // +
    //            sizeof(TSTR2) + sizeof(TSTR3)) *
    //           REPEAT_TIMES);
    for (int i = 0; i < REPEAT_TIMES; i++) {
        s += tmp_fsc + str1 + TSTR1 + str2 + TSTR2 + str3 + TSTR3;
    }
    return nullptr;
}

void *test_rope(void *args) {
    __gnu_cxx::crope result;
    for (int i = 0; i < REPEAT_TIMES; i++) {
        result.append(str1.c_str());
        result.append(TSTR1);
        result.append(str2.c_str());
        result.append(TSTR2);
        result.append(str3.c_str());
        result.append(TSTR3);
    }
    std::string s = result.c_str();
    return nullptr;
}

void *test_tablestring(void *args) {
    table_str_t table(999999999);
    for (int i = 0; i < REPEAT_TIMES; i++) {
        table.add(str1.c_str(), str1.length());
        table.add(TSTR1, sizeof(TSTR1));
        table.add(str2.c_str(), str2.length());
        table.add(TSTR2, sizeof(TSTR2));
        table.add(str3.c_str(), str3.length());
        table.add(TSTR3, sizeof(TSTR3));
    }
    return nullptr;
}

void *test_swiftjson(void *args) {
    SwiftJson json;
    for (int i = 0; i < REPEAT_TIMES; i++) {
        json.append(str1);
        json.append(TSTR1);
        json.append(str2);
        json.append(TSTR2);
        json.append(str3);
        json.append(TSTR3);
    }
    std::string s = json.toString();
    // std::cout<<s<<std::endl;
    return nullptr;
}

std::string ip = "192.168.1.1";
std::string key = "machineCode";

void *test_make(void *args) {
    std::string s;
    s += "{";
    for (int i = 1; i < REPEAT_TIMES; i++) {
        s += R"(, "sendHostAddress":")";
        s += ip;
        s += R"(", ")";
        s += key;
        s += R"(:"1a-2b-3c-4d-5e-6f)";
        s += R"(")";
    }
    s += "}";
    return nullptr;
}

void *test_make_pp(void *args) {
    std::string s;
    s += "{";
    for (int i = 1; i < REPEAT_TIMES; i++) {
        s += R"(, "sendHostAddress":")" + ip + R"(")";
        s += R"(, ")" + key + R"(:")" + "1a-2b-3c-4d-5e-6f" + R"(")";
    }
    s += "}";
    return nullptr;
}

void *test_make_ss(void *args) {
    std::stringstream ss;
    ss << "{";
    for (int i = 1; i < REPEAT_TIMES; i++) {
        ss << R"(, "sendHostAddress":")" << ip << R"(")";
        ss << R"(, ")" + key << R"(:")"
           << "1a-2b-3c-4d-5e-6f"
           << R"(")";
    }
    ss << "}";
    std::string s = ss.str();
    return nullptr;
}

void *test_make_swift(void *args) {
    SwiftJson json;

    json.start();
    for (int i = 0; i < REPEAT_TIMES; i++) {
        json.appendKey("sendHostAddress");
        json.appendValue(ip);
        json.appendKey(key);
        json.appendValue("1a-2b-3c-4d-5e-6f");
    }
    json.end();

    std::string s = json.toString();
    return nullptr;
}

void *test_make_swift_fst(void *args) {
    SwiftJson json;

    json.start();
    for (int i = 0; i < REPEAT_TIMES; i++) {
        json.append(R"(, "sendHostAddress":")");
        json.append(ip);
        json.append(R"(", ")");
        json.append(key);
        json.append(R"(:"1a-2b-3c-4d-5e-6f)");
        json.append(R"(")");
    }
    json.end();

    std::string s = json.toString();
    return nullptr;
}

void *test_make_swift_oop(void *args) {
    SwiftJson json;

    json.start();
    for (int i = 0; i < REPEAT_TIMES; i++) {
        json + R"(, "sendHostAddress":")" + ip + R"(", ")";
        json + key + R"(:"1a-2b-3c-4d-5e-6f)" + R"(")";
        // json += ip;
        // json += R"(", ")";
        // json += key;
        // json += R"(:"1a-2b-3c-4d-5e-6f)";
        // json += R"(")";
    }
    json.end();

    std::string s = json.toString();
    // std::cout << s << std::endl;
    return nullptr;
}

std::string sep_str;

void sep1() {
    SwiftJson json;

    for (int i = 0; i < REPEAT_TIMES; i++) {
        json.append(R"(, "sendHostAddress":")");
        json.append(ip);
        json.append(R"(", ")");
    }

    sep_str.append(json.toString());
}

void sep2() {
    SwiftJson json;

    for (int i = 0; i < REPEAT_TIMES; i++) {
        json.append(key);
        json.append(R"(:"1a-2b-3c-4d-5e-6f)");
        json.append(R"(")");
    }

    sep_str.append(json.toString());
}

void *test_make_swift_sep(void *args) {
    sep_str.append("{");
    sep1();
    sep2();
    sep_str.append("}");
    return nullptr;
}

void *test_append_res(void *args) {
    std::string s;
    s.reserve((str1.length() + str2.length() + str3.length() + sizeof(TSTR1) +
               sizeof(TSTR2) + sizeof(TSTR3)) *
              REPEAT_TIMES);
    for (int i = 0; i < REPEAT_TIMES; i++) {
        s.append(TSTR1);
        s.append(str1);
        s.append(TSTR2);
        s.append(str2);
        s.append(TSTR3);
        s.append(str3);
    }
    return nullptr;
}

void *test_append_ptr(void *args) {
    std::string s;
    // s.reserve((str1.length() + str2.length() + str3.length() + sizeof(TSTR1)
    // +
    //            sizeof(TSTR2) + sizeof(TSTR3)) *
    //           REPEAT_TIMES);
    const char *strs[1000] = {TSTR1,       str1.data(), TSTR2,
                              str2.data(), TSTR3,       str3.data()};
    for (int i = 0; i < REPEAT_TIMES; i++) {
        for (int j = 0; j < 6; ++j) {
            s.append(strs[j]);
        }
    }
    return nullptr;
}

void multi_thread_test(TestFunction &tf) {
    auto start = std::chrono::high_resolution_clock::now();
#if THREAD_COUNT > 1
    pthread_t threads[THREAD_COUNT];
    for (unsigned long long &thread : threads) {
        int ret = pthread_create(&thread, nullptr, tf.func, nullptr);
        if (ret != 0) {
            std::cout << "pthread_create error: error_code=" << ret
                      << std::endl;
        }
    }
    for (unsigned long long thread : threads) {
        pthread_join(thread, nullptr);
    }
#else
    tf.func(nullptr);
#endif
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> elapsed = end - start;
    tf.overhead = elapsed.count();
}

int getChineseCharCount(const std::string &str) {
    int count = 0;
    for (unsigned char c : str) {
        if (c >= 0x80)  // UTF-8 中文字符的第一个字节的最高位为1
            count++;
    }
    return count / 3;  // UTF-8 编码下，每个中文字符占用3个字节
}

void print_result(std::vector<TestFunction> &funcs, unsigned long long size) {
    std::sort(funcs.begin(), funcs.end(),
              [](const TestFunction &a, const TestFunction &b) {
                  return a.overhead < b.overhead;
              });
    for (int i = 0; i < size; ++i) {
        int chineseCharCount = getChineseCharCount(funcs[i].name);
        std::cout << std::setw(16 + chineseCharCount) << std::setfill('-')
                  << std::left << funcs[i].name << std::setw(16) << std::right
                  << "cost: " << funcs[i].overhead << "\t ms" << std::endl;
    }
}

int main() {
    std::vector<TestFunction> string_funcs = {
        TestFunction("stringstream", test_stringstream),
        TestFunction("fmt", test_fmt), TestFunction("fmt_buf", test_fmt_buf),
        TestFunction("snprintf", test_snprintf),
        TestFunction("append", test_append),
        TestFunction("append_reserve", test_append_res),
        TestFunction("append_批量", test_append_ptr),
        TestFunction("append_模板封装", test_append_obo),
        TestFunction("append_短多次", test_append_short),
        TestFunction("add", test_add),
        TestFunction("add_模板封装", test_add_obo),
        TestFunction("add_短多次", test_add_short),
        // TestFunction("ternary_indirect", test_ternary_indirect),
        // TestFunction("ternary_direct", test_ternary_direct),
        // TestFunction("assign", test_assign),
        // TestFunction("noassign", test_noappend),
        TestFunction("sonic", test_sonic),
        TestFunction("sonic_短多次", test_sonic_obo),
        TestFunction("fast_const", test_fast_concat),
        TestFunction("rope", test_rope),
        TestFunction("tablestring", test_tablestring),
        TestFunction("swiftjson", test_swiftjson),
        // TestFunction("make", test_make),
        // TestFunction("make_pp", test_make_pp),
        // TestFunction("make_ss", test_make_ss),
        // TestFunction("make_swift", test_make_swift),
        // TestFunction("make_swift_fst", test_make_swift_fst),
        // TestFunction("make_swift_oop", test_make_swift_oop),
        // TestFunction("make_swift_sep", test_make_swift_sep),
    };

    // for (TestFunction &nf : string_funcs) {
    //     multi_thread_test(nf);
    // }
    // print_result(string_funcs, string_funcs.size());

    // SwiftJson json;
    // std::string s;
    // std::string a = "123";
    // json + "abc" + a;
    // s = json.toString();
    // std::cout << s << std::endl;

    // MyString str("Hello, ");
    // str += "World!";
    // str.append(" How are you?");
    // std::cout << str.c_str() << std::endl;  // 输出 "Hello, World! How are you?"

    std::string s = "abcdef";
    Fundation::Buffer<char> buf(8);
    buf.append("123", 3);
    buf.append(s.data(), s.length());
    buf.append('\n');
    std::string ss(buf.begin(), buf.end());
    std::cout << ss << std::endl;
}
