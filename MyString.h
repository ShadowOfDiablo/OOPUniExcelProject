#ifndef MYSTRING_H
#define MYSTRING_H

#include <iostream>
#include <cstring>
#include "Vector.hpp" 

class MyString;
std::ostream& operator<<(std::ostream& os, const MyString& s);

class MyString {
public:
    static const size_t npos = static_cast<size_t>(-1);

    MyString();
    MyString(const char* s);
    MyString(const MyString& other);
    ~MyString();

    MyString& operator=(const MyString& other);
    MyString& operator+=(const MyString& other);
    friend MyString operator+(const MyString& lhs, const MyString& rhs);

    char operator[](int index) const;
    int length() const;
    int getSize() const;
    const char* c_str() const;

    size_t find(char u8_delimiter, size_t u32_start = 0U) const;
    MyString substr(size_t u32_start, size_t u32_length = npos) const;

    Vector<MyString> split(const MyString& p_myString, char u8_delimiter);
    Vector<MyString> split(char cDelimiter) const;
    MyString(double value);
    bool endsWith(const char* p_suffix) const;
    static MyString join(const Vector<MyString>& c_tokens, char c_separator, size_t u32_startIndex);
    bool startsWith(const MyString& cPrefix) const;

    int find(const MyString& str) const;
    static MyString trim(const MyString& s);
private:
    char* data;
    int len;
    void allocate_and_copy(const char* s);
};

bool operator==(const MyString& lhs, const MyString& rhs);
bool operator!=(const MyString& lhs, const MyString& rhs);
bool operator<(const MyString& lhs, const MyString& rhs);
bool operator>(const MyString& lhs, const MyString& rhs);
bool operator<=(const MyString& lhs, const MyString& rhs);
bool operator>=(const MyString& lhs, const MyString& rhs);
void trimMyString(MyString& str);

#endif // MYSTRING_H
