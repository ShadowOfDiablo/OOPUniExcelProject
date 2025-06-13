#pragma once
#include "Cell.h"
#include "MyString.h"
class FormulaCell : public Cell {
private:
    MyString c_formula;

public:
    FormulaCell(const MyString& formula);

    Cell* clone() const override;
    MyString toString() const override;
    Type getType() const override;
};
