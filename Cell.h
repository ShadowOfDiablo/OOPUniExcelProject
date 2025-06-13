#pragma once
#include <memory>
#include "MyString.h"

class IntCell;
class DoubleCell;
class MyStringCell;
class BoolCell;

class Cell {
public:
    enum class Type { INT, DOUBLE, STRING, BOOL, EMPTY,FORMULA };
private:
    MyString u8Name;
    Type value = Type::EMPTY;
public:
    Cell();
    Cell(MyString name, Type val);
    virtual Cell* clone() const = 0;
    virtual MyString toString() const = 0;
    virtual ~Cell() = default;
    virtual Type getType() const = 0;

	const MyString& getName() const { return u8Name; }

    static std::unique_ptr<Cell> createIntCell(int value);
    static std::unique_ptr<Cell> createDoubleCell(double value);
    static std::unique_ptr<Cell> createStringCell(const MyString& value);
    static std::unique_ptr<Cell> createBoolCell(bool value);
};