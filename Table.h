#pragma once
#include <iostream>
#include "MyString.h"
#include "Cell.h"
#include "BoolCell.h"
#include "IntCell.h"
#include "DoubleCell.h"
#include "MyStringCell.h"
#include "FormulaEvaluator.h"

enum Alignment { left, center, right };

class Table {
private:
    static Table* instance;
    long long unsigned llu8totalRows = 1;
    long long unsigned llu8totalCols = 1;
    long long unsigned llu8initialTableRows = 1;
    long long unsigned llu8initialTableCols = 1;
    long long unsigned llu8maxTableRows = 100;
    long long unsigned llu8maxTableCols = 100;
    long long unsigned llu8autoFit = false;
    long long unsigned llu8visibleCellSymbols = 20;
    Alignment s_initialAlignment = left;
    bool bclearConsoleAfterCommand = false;
    bool bautofit = false;
    MyString* p_configFilePath = nullptr;
    Vector<Vector<Cell*>> p_table;

    void ensureCapacity(size_t row, size_t col);

    Table();

public:
    static Table* getInstance();
    ~Table();
    void initializeTable();
    void parseConfigLine(MyString& line);
    void setInitialTableRows(long long unsigned rows);
    void setInitialTableCols(long long unsigned cols);
    Cell* getCell(size_t u32Row, size_t u32Col) const;
    void loadTableFromFile();
    void setCell(size_t row, size_t col, Cell* cell);
    void run();
    void insertValue(const MyString& c_command);
    void saveTable();
    void deleteCell(const MyString& c_command);
    void debugTestScript();
    void referenceCell(const MyString& c_command);
    double sumCells(const MyString& c_command);
    void averageCells(const MyString& c_command);
    void minCells(const MyString& c_command);
    void maxCells(const MyString& c_command);
    MyString concatCells(const MyString& c_command);
    void substrCell(const MyString& c_command);
    void openTable(const MyString& command);
    void createTable(const MyString& command);
    void printTable() const;
};

Alignment stoal(const MyString& alignment);
unsigned stoul(MyString str);
