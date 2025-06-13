#pragma once
#include "Cell.h"
class IntCell : public Cell
{
private:
    int u32_value = 0;
public:
    IntCell();
    IntCell(int value);
    Type getType() const override;
    int getValue() const;
    void setValue(int newValue);
    MyString toString() const;
    Cell* clone() const override;
};