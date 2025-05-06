#include "MyString.h"

enum class Type { INT, DOUBLE, STRING, BOOL, EMPTY };

class Cell {
public:
    virtual ~Cell();
    virtual Type getType() const = 0;
    static std::unique_ptr<Cell> createIntCell(int value);
    static std::unique_ptr<Cell> createDoubleCell(double value);
    static std::unique_ptr<Cell> createStringCell(MyString value);
    static std::unique_ptr<Cell> createBoolCell(bool value);
    static std::unique_ptr<Cell> createEmptyCell();
};