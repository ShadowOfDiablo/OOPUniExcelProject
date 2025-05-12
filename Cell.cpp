#include "Cell.h"
#include "IntCell.h"
#include "DoubleCell.h"
#include "MyStringCell.h"
#include "BoolCell.h"

std::unique_ptr<Cell> Cell::createIntCell(int value) {
    return std::make_unique<IntCell>(value);
}

std::unique_ptr<Cell> Cell::createDoubleCell(double value) {
    return std::make_unique<DoubleCell>(value);
}

std::unique_ptr<Cell> Cell::createStringCell(const MyString& value) {
    return std::make_unique<MyStringCell>(value);
}

std::unique_ptr<Cell> Cell::createBoolCell(bool value) {
    return std::make_unique<BoolCell>(value);
}