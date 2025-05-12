// MyStringCell.cpp
#include "MyStringCell.h"

MyStringCell::MyStringCell(const MyString& value) : value(value) {}

Cell::Type MyStringCell::getType() const {
    return Type::STRING;  
}

const MyString& MyStringCell::getValue() const {
    return value;
}

void MyStringCell::setValue(const MyString& newValue) {
    value = newValue;
}