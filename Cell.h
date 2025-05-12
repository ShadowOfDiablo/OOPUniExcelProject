#pragma once
#include <memory>
#include "MyString.h"

class IntCell;
class DoubleCell;
class MyStringCell;
class BoolCell;

class Cell {
public:
    enum class Type { INT, DOUBLE, STRING, BOOL, EMPTY };
private:
    Type value = Type::EMPTY;
public:

    virtual ~Cell() = default;
    virtual Type getType() const = 0;

    // Factory methods should be static and return unique_ptr
    static std::unique_ptr<Cell> createIntCell(int value);
    static std::unique_ptr<Cell> createDoubleCell(double value);
    static std::unique_ptr<Cell> createStringCell(const MyString& value);
    static std::unique_ptr<Cell> createBoolCell(bool value);
};