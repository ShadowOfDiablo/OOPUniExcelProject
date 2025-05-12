#pragma once
#include "Cell.h"
#include "MyString.h"

class MyStringCell : public Cell {
    MyString value;
public:
    explicit MyStringCell(const MyString& value);
    Type getType() const override;
    const MyString& getValue() const;
    void setValue(const MyString& newValue);
};