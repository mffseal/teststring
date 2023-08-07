#include "fast_string_concatenator.hpp"
#include "sonic/dom/generic_document.h"
#include "sonic/sonic.h"
#include <algorithm>
#include <chrono>
#include <ext/rope>
#include <fmt/format.h>
#include <iomanip>
#include <iostream>
#include <llvm-14/llvm/ADT/Twine.h>
#include <pthread.h>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include "table_str.hh"

#define THREAD_COUNT 1
#define REPEAT_TIMES 10000

template<typename... Args>
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
  appendToString(base, args...); // recursive call using remaining arguments
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
std::string str3 = "78978978978978978978978978978978978978978978978978978978978"
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
  s.reserve((str1.length() + str2.length() + str3.length() + sizeof(TSTR1) + sizeof(TSTR2)+sizeof(TSTR3)) * REPEAT_TIMES);
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
  s.reserve((str1.length() + str2.length() + str3.length() + sizeof(TSTR1) + sizeof(TSTR2)+sizeof(TSTR3)) * REPEAT_TIMES);
  for (int i = 0; i < REPEAT_TIMES; i++) {
    s = fmt::format("{}{}{}{}{}{}\n", TSTR1, str1, TSTR2, str2, "hig", str3);
  }
  return nullptr;
}

void *test_fmt_buf(void *args) {
  fmt::memory_buffer buf;
  buf.reserve((str1.length() + str2.length() + str3.length() + sizeof(TSTR1) + sizeof(TSTR2)+sizeof(TSTR3)) * REPEAT_TIMES);
  for (int i = 0; i < REPEAT_TIMES; i++) {
    fmt::format_to(std::back_inserter(buf), "{}{}{}{}{}{}\n", TSTR1, str1,
                   TSTR2, str2, "hig", str3);
  }
  fmt::to_string(buf);
  return nullptr;
}

void *test_add(void *args) {
  std::string s;
  s.reserve((str1.length() + str2.length() + str3.length() + sizeof(TSTR1) + sizeof(TSTR2)+sizeof(TSTR3)) * REPEAT_TIMES);
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
  s.reserve((str1.length() + str2.length() + str3.length() + sizeof(TSTR1) + sizeof(TSTR2)+sizeof(TSTR3)) * REPEAT_TIMES);
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
  s.reserve((str1.length() + str2.length() + str3.length() + sizeof(TSTR1) + sizeof(TSTR2)+sizeof(TSTR3)) * REPEAT_TIMES);
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
  for (int i = 0; i < REPEAT_TIMES; i++) {
    wb.Push(TSTR1, sizeof TSTR1);
    wb.Push(str1.data(), 30);
    wb.Push(TSTR2, 11);
    wb.Push(str2.data(), 63);
    wb.Push(TSTR3, 11);
    wb.Push(str3.data(), 96);
  }
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
        s = "Hello, " "World!" " How " "are " "you?";
    }
    return nullptr;
}

void *test_fast_concat(void *args) {
  stlsoft::fsc_seed tmp_fsc;
  std::string s;
  s.reserve((str1.length() + str2.length() + str3.length() + sizeof(TSTR1) + sizeof(TSTR2)+sizeof(TSTR3)) * REPEAT_TIMES);
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

void multi_thread_test(TestFunction &tf) {
  auto start = std::chrono::high_resolution_clock::now();
#if THREAD_COUNT > 1
  pthread_t threads[THREAD_COUNT];
  for (unsigned long long &thread : threads) {
    int ret = pthread_create(&thread, nullptr, tf.func, nullptr);
    if (ret != 0) {
      std::cout << "pthread_create error: error_code=" << ret << std::endl;
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

void print_result(std::vector<TestFunction> &funcs, unsigned long long size) {
  std::sort(funcs.begin(), funcs.end(),
            [](const TestFunction &a, const TestFunction &b) {
              return a.overhead < b.overhead;
            });
  for (int i = 0; i < size; ++i) {
    std::cout << std::setw(16) << std::setfill('-') << std::left
              << funcs[i].name << std::setw(16) << std::right
              << "cost: " << funcs[i].overhead << "\t ms" << std::endl;
  }
}

int main() {
  std::vector<TestFunction> string_funcs = {
      TestFunction("stringstream", test_stringstream),
      TestFunction("fmt", test_fmt),
      TestFunction("fmt_buf", test_fmt_buf),
      TestFunction("snprintf", test_snprintf),
      TestFunction("append", test_append),
      TestFunction("append_obo", test_append_obo),
      TestFunction("append_short", test_append_short),
      TestFunction("add", test_add),
      TestFunction("add_obo", test_add_obo),
      TestFunction("add_short", test_add_short),
    //   TestFunction("ternary_indirect", test_ternary_indirect),
    //   TestFunction("ternary_direct", test_ternary_direct),
    //   TestFunction("assign", test_assign),
    //   TestFunction("noassign", test_noappend),
      TestFunction("sonic", test_sonic),
      TestFunction("fast_const", test_fast_concat),
      TestFunction("rope", test_rope),
      TestFunction("tablestring", test_tablestring),
  };

  for (TestFunction &nf : string_funcs) {
    multi_thread_test(nf);
  }

  print_result(string_funcs, string_funcs.size());
}
