#pragma once
#include "Cell.h"
#include "MyString.h"
#include <vector>
#include <fstream>
#include <stdexcept>

class Table {
private:
    size_t initialTableRows;
    size_t initialTableCols;
    size_t maxTableRows;
    size_t maxTableCols;
    bool autoFit;
    size_t visibleCellSymbols;
    MyString initialAlignment;
    bool clearConsoleAfterCommand;

    std::vector<std::vector<std::unique_ptr<Cell>>> cells;

    void parseConfigLine(MyString& line);
    void initializeTable();
    void validateConfiguration() const;
    void trimMyString(MyString& str);

public:
    explicit Table(const MyString& configFilePath);

    size_t getInitialRows() const { return initialTableRows; }
    size_t getInitialCols() const { return initialTableCols; }
    size_t getMaxRows() const { return maxTableRows; }
    size_t getMaxCols() const { return maxTableCols; }
    bool isAutoFit() const { return autoFit; }
    size_t getVisibleSymbols() const { return visibleCellSymbols; }
    const MyString& getAlignment() const { return initialAlignment; }
    bool shouldClearConsole() const { return clearConsoleAfterCommand; }

    void resize(size_t rows, size_t cols);
    void setCell(size_t row, size_t col, std::unique_ptr<Cell> cell);
    const Cell* getCell(size_t row, size_t col) const;
    void display() const;
};