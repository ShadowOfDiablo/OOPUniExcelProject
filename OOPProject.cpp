#include "Table.h"
#include <iostream>
#define TEST_RESULT(testName, expected, got)                          \
    do {                                                              \
        if ((expected) == (got))                                      \
            std::cout << testName << " PASS\n";                       \
        else                                                          \
            std::cout << testName << " FAIL: expected [" << expected  \
                      << "] but got [" << got << "]\n";                \
    } while (0)

void testSUBSTR() {
    std::cout << "\n=== Testing SUBSTR ===\n";
    // Valid substring extraction
    {
        MyString formula = "=SUBSTR(\"Hello World\", 6, 5)";
        MyString result = evaluateFormula(formula, Table::getInstance());
        TEST_RESULT("SUBSTR valid", "World", result);
    }
    // Negative start index → error
    {
        MyString formula = "=SUBSTR(\"Hello World\", -6, 5)";
        MyString result = evaluateFormula(formula, Table::getInstance());
        TEST_RESULT("SUBSTR negative start", "#VALUE!", result);
    }
    // Negative count → error
    {
        MyString formula = "=SUBSTR(\"Hello World\", 3, -2)";
        MyString result = evaluateFormula(formula, Table::getInstance());
        TEST_RESULT("SUBSTR negative count", "#VALUE!", result);
    }
    // Start index beyond string length → empty string
    {
        MyString formula = "=SUBSTR(\"Hello\", 10, 2)";
        MyString result = evaluateFormula(formula, Table::getInstance());
        TEST_RESULT("SUBSTR start beyond length", "", result);
    }
}

void testIF() {
    std::cout << "\n=== Testing IF ===\n";
    // Literal condition true:
    {
        MyString formula = "=IF(\"true\", \"Yes\", \"No\")";
        MyString result = evaluateFormula(formula, Table::getInstance());
        TEST_RESULT("IF literal true", "Yes", result);
    }
    // Literal condition false:
    {
        MyString formula = "=IF(\"false\", \"Yes\", \"No\")";
        MyString result = evaluateFormula(formula, Table::getInstance());
        TEST_RESULT("IF literal false", "No", result);
    }
    // Invalid condition → error
    {
        MyString formula = "=IF(\"maybe\", \"Yes\", \"No\")";
        MyString result = evaluateFormula(formula, Table::getInstance());
        TEST_RESULT("IF invalid condition", "#VALUE!", result);
    }
    // Condition from cell reference: set A1 = "true"
    {
        Table* t = Table::getInstance();
        t->setCell(0, 0, new MyStringCell("true")); // A1
        MyString formula = "=IF(A1, \"Yes\", \"No\")";
        MyString result = evaluateFormula(formula, t);
        TEST_RESULT("IF cell condition true", "Yes", result);
    }
}

void testSUM() {
    std::cout << "\n=== Testing SUM ===\n";
    Table* t = Table::getInstance();
    // Set up cells: A1 = 10, B1 = 20
    t->setCell(0, 0, new IntCell(10));  // A1
    t->setCell(0, 1, new IntCell(20));  // B1
    MyString formula = "=SUM(A1:B1)";
    MyString result = evaluateFormula(formula, t);
    TEST_RESULT("SUM test", "30", result);
}

void testAVERAGE() {
    std::cout << "\n=== Testing AVERAGE ===\n";
    Table* t = Table::getInstance();
    // Using same A1 (10) and B1 (20) from previous test
    MyString formula = "=AVERAGE(A1:B1)";
    MyString result = evaluateFormula(formula, t);
    // Average of 10 and 20 is 15
    TEST_RESULT("AVERAGE test", "15", result);
}

void testMIN() {
    std::cout << "\n=== Testing MIN ===\n";
    Table* t = Table::getInstance();
    // Set up cells for MIN: A1 = 10, A2 = 5, A3 = 15.
    t->setCell(0, 0, new IntCell(10));  // A1
    t->setCell(1, 0, new IntCell(5));   // A2
    t->setCell(2, 0, new IntCell(15));  // A3
    MyString formula = "=MIN(A1:A3)";
    MyString result = evaluateFormula(formula, t);
    TEST_RESULT("MIN test", "5", result);
}

void testMAX() {
    std::cout << "\n=== Testing MAX ===\n";
    Table* t = Table::getInstance();
    // Set up cells for MAX: B1 = 25, B2 = 35, B3 = 5.
    t->setCell(0, 1, new IntCell(25));  // B1
    t->setCell(1, 1, new IntCell(35));  // B2
    t->setCell(2, 1, new IntCell(5));   // B3
    MyString formula = "=MAX(B1:B3)";
    MyString result = evaluateFormula(formula, t);
    TEST_RESULT("MAX test", "35", result);
}

void testLEN() {
    std::cout << "\n=== Testing LEN ===\n";
    // Test with literal string
    {
        MyString formula = "=LEN(\"Hello\")";
        MyString result = evaluateFormula(formula, Table::getInstance());
        TEST_RESULT("LEN literal", "5", result);
    }
    // Test with cell reference: set C1 = "World"
    {
        Table* t = Table::getInstance();
        t->setCell(0, 2, new MyStringCell("World")); // C1
        MyString formula = "=LEN(C1)";
        MyString result = evaluateFormula(formula, t);
        TEST_RESULT("LEN cell reference", "5", result);
    }
}

void testCONCAT() {
    std::cout << "\n=== Testing CONCAT ===\n";
    Table* t = Table::getInstance();
    // Set up a range for CONCAT (using our internal convention: first character = row letter);
    // Let's use cells: B2 = "7", B3 = "9", C2 = "OOP", C3 = "2".
    // With our desired behavior we want only numeric cells concatenated, so expected output is "7;9;2"
    t->setCell(1, 1, new MyStringCell("7"));   // B2
    t->setCell(2, 1, new MyStringCell("9"));   // B3
    t->setCell(1, 2, new MyStringCell("OOP"));  // C2
    t->setCell(2, 2, new MyStringCell("2"));    // C3
    MyString formula = "=CONCAT(B2:C3, \";\")";
    MyString result = evaluateFormula(formula, t);
    TEST_RESULT("CONCAT test", "7;9;2", result);
}
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
    //testSUBSTR();
    //testIF();
    //testSUM();
    //testAVERAGE();
    //testMIN();
    //testMAX();
    //testLEN();
    //testCONCAT();
    char c_response[4];
    std::cout << "Would you like to load a table from a file? (yes/no): ";
    std::cin.getline(c_response, sizeof(c_response));

    if (MyString::trim(c_response) == "yes") {
        p_table->loadTableFromFile();
    }
    else {
        p_table->setInitialTableRows(3);
        p_table->setInitialTableCols(3);
        p_table->initializeTable();
    }
    //p_table->debugTestScript();
	p_table->printTable();
    p_table->run();
    return 0;
}


