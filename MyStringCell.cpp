#include "MyStringCell.h"

MyStringCell::MyStringCell(const MyString& value) : c_value(value) {}

Cell::Type MyStringCell::getType() const {
    return Type::STRING;  
}

const MyString& MyStringCell::getValue() const {
    return c_value;
}

void MyStringCell::setValue(const MyString& newValue) {
    c_value = newValue;
}
MyString MyStringCell::toString() const {
    return c_value;
}

Cell* MyStringCell::clone() const {
    return new MyStringCell(c_value);
}
