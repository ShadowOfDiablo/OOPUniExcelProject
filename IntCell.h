#pragma once
#include "Cell.h"
class IntCell : public Cell
{
private:
    int value;
public:
	explicit IntCell(int value) : value(value) {};
    Type getType() const override { return Type::INT; }
    int getValue() const { return value; }
    void setValue(int newValue) { value = newValue; }
};