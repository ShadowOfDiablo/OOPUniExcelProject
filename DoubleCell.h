#pragma once
#include "Cell.h"
class DoubleCell : public Cell
{
private:
    double f64_value = 0.0;
public:
    DoubleCell();
    DoubleCell(double value);
    Type getType() const override;
    double getValue() const;
    void setValue(double newValue);
    MyString toString() const;
    Cell* clone() const override;

};