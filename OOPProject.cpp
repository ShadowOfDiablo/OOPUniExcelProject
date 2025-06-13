#include "Table.h"
#include <iostream>

void testTableCreation() {
    Table* p_table = Table::getInstance();

    p_table->setInitialTableRows(3);
    p_table->setInitialTableCols(3);

    p_table->initializeTable();
    std::cout << "Table initialized successfully.\n";
    p_table->printTable();
}

void testCellInsertion() {
    Table* p_table = Table::getInstance();


    p_table->setCell(0, 0, new IntCell(42));
    p_table->setCell(0, 1, new DoubleCell(3.14));
    p_table->setCell(0, 2, new IntCell(8));


    p_table->setCell(1, 0, new IntCell(7));
    p_table->setCell(1, 1, new DoubleCell(2.71));
    p_table->setCell(1, 2, new IntCell(99));


    p_table->setCell(2, 0, new DoubleCell(1.618));
    p_table->setCell(2, 1, new IntCell(123));
    p_table->setCell(2, 2, new DoubleCell(0.577));

    std::cout << "Cells inserted successfully.\n";
    p_table->printTable();
}

void testSavingTable() {
    Table* p_table = Table::getInstance();
    p_table->saveTable();
    std::cout << "Table saved successfully.\n";
}

int main() {
    std::cout << "Running table...\n";

    Table* p_table = Table::getInstance();

    /*char c_response[4];
    std::cout << "Would you like to load a table from a file? (yes/no): ";
    std::cin.getline(c_response, sizeof(c_response));

    if (MyString::trim(c_response) == "yes") {
        p_table->loadTableFromFile();
    }
    else {
        p_table->setInitialTableRows(3);
        p_table->setInitialTableCols(3);
        p_table->initializeTable();
    }*/
    p_table->debugTestScript();
	p_table->printTable();
    p_table->run();
    return 0;
}


