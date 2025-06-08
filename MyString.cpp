#define _CRT_SECURE_NO_WARNINGS
#include "Vector.hpp"
#include "MyString.h"
#include <iostream>
#include <cstring>

MyString::MyString() : data(nullptr), len(0) {
    allocate_and_copy("");
}

MyString::MyString(const char* s) : data(nullptr), len(0) {
    if (s) {
        allocate_and_copy(s);
    }
    else {
        allocate_and_copy("");
    }
}

MyString::MyString(const MyString& other) : data(nullptr), len(0) {
    allocate_and_copy(other.data);
}

MyString::~MyString() {
    delete[] data;
}

MyString& MyString::operator=(const MyString& other) {
    if (this != &other) {
        delete[] data;
        allocate_and_copy(other.data);
    }
    return *this;
}

MyString& MyString::operator+=(const MyString& other) {
    int new_len = len + other.len;
    char* new_data = new char[static_cast<size_t>(new_len) + 1U];
    (void)strcpy(new_data, data);
    (void)strcat(new_data, other.data);
    delete[] data;
    data = new_data;
    len = new_len;
    return *this;
}

char MyString::operator[](int index) const {
    char u8_ret_val = '\0';
    if ((index >= 0) && (index < len)) {
        u8_ret_val = data[static_cast<size_t>(index)];
    }
    return u8_ret_val;
}
int MyString::length() const {
    return len;
}
int MyString::getSize() const {
    return length();
}

size_t MyString::find(char u8_delimiter, size_t u32_start) const {
    for (size_t u32_idx = u32_start; u32_idx < static_cast<size_t>(len); ++u32_idx) {
        if (data[u32_idx] == u8_delimiter) {
            return u32_idx;
        }
    }
    return npos;
}

MyString MyString::substr(size_t u32_start, size_t u32_length) const {
    if (u32_start >= static_cast<size_t>(len)) {
        return MyString();
    }

    size_t u32_end;
    if (u32_length == npos || u32_start + u32_length > static_cast<size_t>(len)) {
        u32_end = static_cast<size_t>(len);
    }
    else {
        u32_end = u32_start + u32_length;
    }

    size_t sub_length = u32_end - u32_start;
    char* buffer = new char[sub_length + 1];

    for (size_t i = 0; i < sub_length; i++) {
        buffer[i] = data[u32_start + i];
    }
    buffer[sub_length] = '\0';

    MyString result(buffer);
    delete[] buffer;
    return result;
}


Vector<MyString> MyString::split(const MyString& p_myString, char u8_delimiter) {
    Vector<MyString> c_parts;
    size_t u32_start = 0U, u32_end = 0U;
    while ((u32_end = p_myString.find(u8_delimiter, u32_start)) != MyString::npos) {
        c_parts.push_back(p_myString.substr(u32_start, u32_end - u32_start));
        u32_start = u32_end + 1U;
    }
    c_parts.push_back(p_myString.substr(u32_start));
    return c_parts;
}

int MyString::find(const MyString& str) const {
    int u32_ret_val = -1;
    if (str.length() == 0) {
        u32_ret_val = 0;
    }
    else if (len >= str.length()) {
        const char* found_ptr = strstr(data, str.c_str());
        if (found_ptr != nullptr) {
            u32_ret_val = static_cast<int>(found_ptr - data);
        }
    }
    return u32_ret_val;
}

MyString MyString::trim(const MyString& s) {
    int start = 0;
    while (start < s.length() && (s[start] == ' ' || s[start] == '\n' || s[start] == '\r'))
        start++;
    int end = s.length() - 1;
    while (end >= 0 && (s[end] == ' ' || s[end] == '\n' || s[end] == '\r'))
        end--;
    if (start > end)
        return MyString("");
    return s.substr(start, end - start + 1);
}

const char* MyString::c_str() const {
    return data;
}

MyString operator+(const MyString& lhs, const MyString& rhs) {
    MyString temp(lhs);
    temp += rhs;
    return temp;
}

void MyString::allocate_and_copy(const char* s) {
    len = static_cast<int>(strlen(s));
    data = new char[static_cast<size_t>(len) + 1U];
    (void)strcpy(data, s);
}

std::ostream& operator<<(std::ostream& os, const MyString& s) {
    os << s.c_str();
    return os;
}

bool operator==(const MyString& lhs, const MyString& rhs) {
    return strcmp(lhs.c_str(), rhs.c_str()) == 0;
}

bool operator!=(const MyString& lhs, const MyString& rhs) {
    return !(lhs == rhs);
}

bool operator<(const MyString& lhs, const MyString& rhs) {
    return strcmp(lhs.c_str(), rhs.c_str()) < 0;
}

bool operator>(const MyString& lhs, const MyString& rhs) {
    return strcmp(lhs.c_str(), rhs.c_str()) > 0;
}

bool operator<=(const MyString& lhs, const MyString& rhs) {
    return !(lhs > rhs);
}

bool operator>=(const MyString& lhs, const MyString& rhs) {
    return !(lhs < rhs);
}
