#include "Table.h"
#include <sstream>
#include <iostream>

Table::Table(const MyString& configFilePath) 
{
    initialTableRows = 1;
    initialTableCols = 1;
    maxTableRows = 100;
    maxTableCols = 100;
    autoFit = false;
    visibleCellSymbols = 20;
    initialAlignment = "left";
    clearConsoleAfterCommand = false;

    std::ifstream configFile(configFilePath.c_str());
    if (!configFile.is_open()) {
        throw std::runtime_error("Could not open configuration file");
    }

    MyString line;
    char buffer[1024];
    while (configFile.getline(buffer, 1024)) {
        line = buffer;
        parseConfigLine(line);
    }

    validateConfiguration();
    initializeTable();
}

void Table::trimMyString(MyString& str) {
    while (str.length() > 0 && isspace(str[0])) {
        str = str.substr(1, str.length() - 1);
    }

    while (str.length() > 0 && isspace(str[str.length() - 1])) {
        str = str.substr(0, str.length() - 1);
    }
}

void Table::parseConfigLine(MyString& line) {
    size_t colonPos = line.find(':');
    if (colonPos == MyString::npos) return;

    MyString property = line.substr(0, colonPos);
    MyString value = line.substr(colonPos + 1, line.length() - colonPos - 1);

    // Trim whitespace
    trimMyString(property);
    trimMyString(value);

    if (property == "initialTableRows") {
        initialTableRows = std::stoul(value.c_str());
    }
    else if (property == "initialTableCols") {
        initialTableCols = std::stoul(value.c_str());
    }
    else if (property == "maxTableRows") {
        maxTableRows = std::stoul(value.c_str());
    }
    else if (property == "maxTableCols") {
        maxTableCols = std::stoul(value.c_str());
    }
    else if (property == "autoFit") {
        autoFit = (value == "true");
    }
    else if (property == "visibleCellSymbols") {
        visibleCellSymbols = std::stoul(value.c_str());
    }
    else if (property == "initialAlignment") {
        if (value != "left" && value != "center" && value != "right") {
            throw std::runtime_error("Invalid alignment value");
        }
        initialAlignment = value;
    }
    else if (property == "clearConsoleAfterCommand") {
        clearConsoleAfterCommand = (value == "true");
    }
}

void Table::validateConfiguration() const {
    if (initialTableRows == 0 || initialTableCols == 0 ||
        maxTableRows == 0 || maxTableCols == 0) {
        throw std::runtime_error("Table dimensions must be positive");
    }

    if (initialTableRows > maxTableRows || initialTableCols > maxTableCols) {
        throw std::runtime_error("Initial dimensions cannot exceed maximum dimensions");
    }
}

void Table::initializeTable() {
    cells.resize(initialTableRows);
    for (auto& row : cells) {
        row.resize(initialTableCols);
    }
}