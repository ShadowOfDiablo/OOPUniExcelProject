#include "Cell.h"
#include "IntCell.h"
#include "DoubleCell.h"
#include "MyStringCell.h"
#include "BoolCell.h"

Cell::Cell()
{
	u8Name = nullptr;
	value = Type::EMPTY;
};
Cell::Cell(MyString name, Type val) : u8Name(name), value(Type::EMPTY) 
{
	if (val == Type::INT) {
		value = Type::INT;
	}
	else if (val == Type::DOUBLE) {
		value = Type::DOUBLE;
	}
	else if (val == Type::STRING) {
		value = Type::STRING;
	}
	else if (val == Type::BOOL) {
		value = Type::BOOL;
	}
	else {
		value = Type::EMPTY;
	}
};

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
