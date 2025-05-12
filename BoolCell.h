#pragma once
#include "Cell.h"
class BoolCell : public Cell
{
private:
    bool value = false;
public:
    BoolCell();
    BoolCell(bool value);
    Type getType() const override;
    bool getValue() const;
    void setValue(bool newValue);
};