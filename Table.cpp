﻿#define _CRT_SECURE_NO_WARNINGS
#include "Table.h"
#include "Cell.h"
#include "IntCell.h"
#include "DoubleCell.h"
#include "MyStringCell.h"
#include "FormulaCell.h"
#include "FormulaEvaluator.h"
#include "MyString.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cctype>
#include <cstring>

Table* Table::instance = nullptr;

void Table::ensureCapacity(size_t row, size_t col) {
	if (row < llu8totalRows && col < llu8totalCols)
		return;
	size_t newRows = llu8totalRows;
	size_t newCols = llu8totalCols;
	if (row >= llu8totalRows)
		newRows = row + 1;
	if (col >= llu8totalCols)
		newCols = col + 1;
	if (newRows > llu8maxTableRows)
		newRows = llu8maxTableRows;
	if (newCols > llu8maxTableCols)
		newCols = llu8maxTableCols;
	Vector<Vector<Cell*>> temp;
	for (size_t i = 0; i < newRows; i++) {
		Vector<Cell*> newRow(newCols);
		for (size_t j = 0; j < newCols; j++) {
			newRow.push_back(nullptr);
		}
		temp.push_back(newRow);
	}
	for (size_t i = 0; i < llu8totalRows; i++) {
		for (size_t j = 0; j < llu8totalCols; j++) {
			temp[i][j] = p_table[i][j];
		}
	}
	p_table = temp;
	llu8totalRows = newRows;
	llu8totalCols = newCols;
}

Table::Table() 
{
	initializeTable();
}

Table::~Table() {
	for (size_t i = 0; i < llu8totalRows; i++) {
		for (size_t j = 0; j < llu8totalCols; j++) {
			delete p_table[i][j];
		}
	}
}

Table* Table::getInstance() {
	if (!instance)
		instance = new Table();
	return instance;
}

void Table::initializeTable() {
	p_table = Vector<Vector<Cell*>>();

	// Create exactly llu8initialTableRows rows.
	for (size_t i = 0; i < llu8initialTableRows; i++) {
		Vector<Cell*> tmp;
		for (size_t j = 0; j < llu8initialTableCols; j++) {
			tmp.push_back(nullptr);
		}
		p_table.push_back(tmp);
	}

	llu8totalRows = llu8initialTableRows;
	llu8totalCols = llu8initialTableCols;
}


void Table::parseConfigLine(MyString& line) {
	size_t pos = line.find(':');
	if (pos == MyString::npos) {
		std::cout << "Config error: " << line.c_str() << "\n";
		exit(1);
	}
	MyString key = line.substr(0, pos);
	MyString value = MyString::trim(line.substr(pos + 1));
	if (key == "initialTableRows")
		llu8initialTableRows = stoul(value);
	else if (key == "initialTableCols")
		llu8initialTableCols = stoul(value);
	else if (key == "maxTableRows")
		llu8maxTableRows = stoul(value);
	else if (key == "maxTableCols")
		llu8maxTableCols = stoul(value);
	else if (key == "autoFit")
		llu8autoFit = (value == "true");
	else if (key == "visibleCellSymbols")
		llu8visibleCellSymbols = stoul(value);
	else if (key == "initialAlignment")
		s_initialAlignment = stoal(value);
	else if (key == "clearConsoleAfterCommand")
		bclearConsoleAfterCommand = (value == "true");
	else {
		std::cout << "Config error: " << line.c_str() << "\n";
		exit(1);
	}
}

void Table::setInitialTableRows(long long unsigned rows) {
	llu8initialTableRows = rows;
}

void Table::setInitialTableCols(long long unsigned cols) {
	llu8initialTableCols = cols;
}

Cell* Table::getCell(size_t u32Row, size_t u32Col) const {
	if (u32Row < llu8totalRows && u32Col < llu8totalCols)
		return p_table[u32Row][u32Col];
	return nullptr;
}

void Table::loadTableFromFile() {
	char buf[256];
	std::cout << "Enter file name: ";
	std::cin.getline(buf, sizeof(buf));
	std::ifstream file(buf);
	if (!file.is_open()) {
		std::cerr << "File error: " << buf << "\n";
		return;
	}
	char line[512];
	while (file.getline(line, sizeof(line))) {
		MyString lineStr(line);
		lineStr = MyString::trim(lineStr);
		if (lineStr.getSize() == 0)
			continue;
		char rowLabel = lineStr[0];
		size_t currentRow = rowLabel - 'A';

		size_t braceStart = lineStr.find('{');
		size_t braceEnd = lineStr.find('}');
		if (braceStart == MyString::npos || braceEnd == MyString::npos)
			continue;

		MyString cellsStr = lineStr.substr(braceStart + 1, braceEnd - braceStart - 1);
		Vector<MyString> cellTokens = cellsStr.split(',');

		for (size_t i = 0; i < cellTokens.getSize(); i++) {
			MyString token = MyString::trim(cellTokens[i]);
			if (token.getSize() < 2)
				continue;
			if (!token.startsWith("[") || !token.endsWith("]"))
				continue;
			token = token.substr(1, token.getSize() - 2);

			size_t colonPos = token.find(':');
			if (colonPos == MyString::npos)
				continue;
			MyString addr = MyString::trim(token.substr(0, colonPos));
			MyString val = MyString::trim(token.substr(colonPos + 1));

			if (addr.getSize() < 2)
				continue;

			char addrLetter = addr[0];
			size_t addrRow = addrLetter - 'A';
			int colNum = 0;
			try {
				colNum = stoi(addr.substr(1));
			}
			catch (...) {
				continue;
			}
			size_t addrCol = static_cast<size_t>(colNum) - 1;

			if (addrRow != currentRow)
				continue;

			try {
				if (val.getSize() == 0) {
					setCell(addrRow, addrCol, nullptr);
				}
				else if (val.startsWith("=")) 
				{
					setCell(addrRow, addrCol, new FormulaCell(val));
				}
				else if (val.startsWith("\"") && val.endsWith("\"")) 
				{
					setCell(addrRow, addrCol, new MyStringCell(val.substr(1, val.getSize() - 2)));
				}
				else if (val == "true" || val == "TRUE" || val == "false" || val == "FALSE") {
					setCell(addrRow, addrCol, new BoolCell(val == "true" || val == "TRUE"));
				}
				else if (bIsNumber(val)) {
					if (val.find('.') != -1) {
						setCell(addrRow, addrCol, new DoubleCell(stod(val)));
					}
					else {
						setCell(addrRow, addrCol, new IntCell(stoi(val)));
					}
				}
				else {
					setCell(addrRow, addrCol, new MyStringCell(val));
				}
			}
			catch (...) {
				continue;
			}
		}
	}
	file.close();
	std::cout << "Table loaded successfully!\n";
}


void Table::setCell(size_t row, size_t col, Cell* p_cell) {
	if (row >= llu8totalRows || col >= llu8totalCols)
		ensureCapacity(row, col);
	delete p_table[row][col];
	p_table[row][col] = p_cell;
}

void Table::insertValue(const MyString& s_cmd) {
	Vector<MyString> tokens = s_cmd.split(' ');
	if (tokens.getSize() < 3U) {
		std::cout << "Invalid insert command.\n";
		return;
	}

	MyString s_target = tokens[0];
	MyString s_value = MyString::join(tokens, ' ', 2U);
	size_t row = s_target[0] - 'A';
	size_t col = stoul(s_target.substr(1)) - 1;

	if (row >= llu8totalRows || col >= llu8totalCols)
		ensureCapacity(row, col);

	if (s_value == "" || s_value == "\"\"") {
		setCell(row, col, nullptr);
		return;
	}

	bool isRef = false;
	if (s_value.getSize() >= 2 && (s_value[0] >= 'A' && s_value[0] <= 'Z')) {
		isRef = true;
		for (size_t i = 1; i < s_value.getSize(); i++) {
			char ch = s_value[i];
			if (ch < '0' || ch > '9') {
				isRef = false;
				break;
			}
		}
	}

	if (isRef) {
		MyString refVal = resolveCell(s_value, static_cast<const Table*>(this));
		if (refVal == "" || refVal == "\"\"") {
			setCell(row, col, nullptr);
			return;
		}
		if (refVal == "true" || refVal == "TRUE") {
			setCell(row, col, new BoolCell(true));
		}
		else if (refVal == "false" || refVal == "FALSE") {
			setCell(row, col, new BoolCell(false));
		}
		else if (refVal.startsWith("\"") && refVal.endsWith("\"")) {
			setCell(row, col, new MyStringCell(refVal.substr(1, refVal.getSize() - 2)));
		}
		else if (refVal.find('.') != -1) {
			try { setCell(row, col, new DoubleCell(stod(refVal))); }
			catch (...) { std::cout << "Invalid double: " << refVal << "\n"; }
		}
		else {
			try { setCell(row, col, new IntCell(stoi(refVal))); }
			catch (...) { std::cout << "Invalid int: " << refVal << "\n"; }
		}
		return;
	}

	if (s_value == "true" || s_value == "TRUE")
		setCell(row, col, new BoolCell(true));
	else if (s_value == "false" || s_value == "FALSE")
		setCell(row, col, new BoolCell(false));
	else if (s_value.startsWith("="))
		setCell(row, col, new FormulaCell(s_value));
	else if (s_value.startsWith("\"") && s_value.endsWith("\""))
		setCell(row, col, new MyStringCell(s_value.substr(1, s_value.getSize() - 2)));
	else if (s_value.find('.') != -1)
		try { setCell(row, col, new DoubleCell(stod(s_value))); }
	catch (...) { std::cout << "Invalid double: " << s_value << "\n"; }
	else
		try { setCell(row, col, new IntCell(stoi(s_value))); }
	catch (...) { std::cout << "Invalid int: " << s_value << "\n"; }
}

void Table::deleteCell(const MyString& s_cmd) {
	Vector<MyString> tokens = s_cmd.split(' ');
	if (tokens.getSize() < 2U) {
		std::cout << "Usage: delete <cell>\n";
		return;
	}
	MyString s_target = tokens[1];
	size_t row = s_target[0] - 'A';
	size_t col = stoul(s_target.substr(1)) - 1;
	if (row < llu8totalRows && col < llu8totalCols) {
		delete p_table[row][col];
		p_table[row][col] = nullptr;
	}
	else
		std::cout << "Invalid cell coordinates.\n";
}

void Table::debugTestScript()
{
	std::cout << "Running debug test script...\n";
	insertValue("A1 insert 2");
	insertValue("C1 insert 9");
	insertValue("B2 insert 7");
	insertValue("C2 insert 2");
	insertValue("B3 insert OOP");
	insertValue("E5 insert TRUE");
	insertValue("A5 = E5");
	insertValue("E3 =SUM(A1,9,B1:C2,B3)");
}


void Table::referenceCell(const MyString& s_cmd) {
	Vector<MyString> tokens = s_cmd.split(' ');
	if (tokens.getSize() < 2U) {
		std::cout << "Usage: reference <cell>\n";
		return;
	}
	MyString s_target = tokens[1];
	size_t row = s_target[0] - 'A';
	size_t col = stoul(s_target.substr(1)) - 1;
	if (row < llu8totalRows && col < llu8totalCols) {
		Cell* cell = getCell(row, col);
		if (cell)
			std::cout << "Value: " << cell->toString() << "\n";
		else
			std::cout << "Cell is empty.\n";
	}
	else
		std::cout << "Cell out of range.\n";
}

double Table::sumCells(const MyString& s_cmd) {
	Vector<MyString> tokens = s_cmd.split(' ');
	if (tokens.getSize() < 2U) {
		std::cout << "Usage: SUM <range>\n";
		return -1;
	}
	Vector<MyString> args;
	args.push_back(tokens[1]);
	std::cout << "SUM = " << sum(args, static_cast<const Table*>(this)) << "\n";
}

void Table::averageCells(const MyString& s_cmd) {
	Vector<MyString> tokens = s_cmd.split(' ');
	if (tokens.getSize() < 2U) {
		std::cout << "Usage: AVERAGE <range>\n";
		return;
	}
	Vector<MyString> args;
	args.push_back(tokens[1]);
	std::cout << "AVERAGE = " << average(args, static_cast<const Table*>(this)) << "\n";
}

void Table::minCells(const MyString& s_cmd) {
	Vector<MyString> tokens = s_cmd.split(' ');
	if (tokens.getSize() < 2U) {
		std::cout << "Usage: MIN <range>\n";
		return;
	}
	Vector<MyString> args;
	args.push_back(tokens[1]);
	std::cout << "MIN = " << min(args, static_cast<const Table*>(this)) << "\n";
}

void Table::maxCells(const MyString& s_cmd) {
	Vector<MyString> tokens = s_cmd.split(' ');
	if (tokens.getSize() < 2U) {
		std::cout << "Usage: MAX <range>\n";
		return;
	}
	Vector<MyString> args;
	args.push_back(tokens[1]);
	double d_max = -1e9;
	size_t row1, col1, row2, col2;
	parseCellRef(tokens[1], row1, col1);
	parseCellRef(tokens[1].substr(tokens[1].find(':') + 1), row2, col2);
	for (size_t r = row1; r <= row2; r++) {
		for (size_t c = col1; c <= col2; c++) {
			Cell* cell = getCell(r, c);
			if (cell) {
				MyString s_val = cell->toString();
				MyString s_eval = isFormula(s_val) ? evaluateFormula(s_val, this) : s_val;
				double d_tmp = parseLiteral(s_eval);
				if (d_tmp > d_max)
					d_max = d_tmp;
			}
		}
	}
	std::cout << "MAX = " << d_max << "\n";
}

// Helper function to check if a string represents a valid number

static int getValueType(const MyString& s) {
	if (s == "true" || s == "TRUE" || s == "false" || s == "FALSE") {
		return 1; // boolean
	}
	if (bIsNumber(s)) {
		return 0; // number
	}
	return 2; // string
}

MyString Table::concatCells(const MyString& c_command) {
	// Expected syntax: "=CONCAT(<range>, <delimiter>)"
	// For example: =CONCAT(B2:C3, ";")
	unsigned int u32Prefix = 8; // length of "=CONCAT("
	if (c_command.getSize() < u32Prefix + 1)
		return "";

	// Extract text inside the parentheses (drop the trailing ')')
	MyString s_inner = c_command.substr(u32Prefix, c_command.getSize() - u32Prefix - 1);
	Vector<MyString> v_params = s_inner.split(',');
	if (v_params.getSize() != 2)
		return "#VALUE!";

	// First parameter is the range (e.g., "B2:C3")
	MyString s_range = MyString::trim(v_params[0]);
	// Second parameter is the delimiter (which may be quoted)
	MyString s_delim = MyString::trim(v_params[1]);
	if (s_delim.startsWith("\"") && s_delim.endsWith("\""))
		s_delim = s_delim.substr(1, s_delim.getSize() - 2);

	if (s_range.find(':') == -1)
		return "#VALUE!";

	size_t posColon = s_range.find(':');
	MyString s_start = MyString::trim(s_range.substr(0, posColon));
	MyString s_end = MyString::trim(s_range.substr(posColon + 1));
	if (s_start.getSize() < 2 || s_end.getSize() < 2)
		return "#VALUE!";

	// Standard spreadsheet notation:
	// The first character represents the column letter (A → 0, B → 1, etc.).
	// The remaining part represents the row number (converted to a 0-based index).
	unsigned int u32StartCol = s_start[0] - 'A';
	unsigned int u32StartRow = (unsigned int)stoi(s_start.substr(1)) - 1;
	unsigned int u32EndCol = s_end[0] - 'A';
	unsigned int u32EndRow = (unsigned int)stoi(s_end.substr(1)) - 1;
	if (u32StartRow > u32EndRow || u32StartCol > u32EndCol)
		return "#VALUE!";

	// Iterate over the rectangular submatrix defined by the range.
	MyString s_result = "";
	bool bFirst = true;
	for (unsigned int r = u32StartRow; r <= u32EndRow; r++) {
		for (unsigned int c = u32StartCol; c <= u32EndCol; c++) {
			Cell* p_cell = getCell(r, c); // Access cell at row r, col c
			if (p_cell) {
				MyString s_val = p_cell->toString();
				// Only include nonempty cells that are entirely numeric.
				if (s_val != "" && bAllDigits(s_val)) {
					if (!bFirst)
						s_result = s_result + s_delim;
					s_result = s_result + s_val;
					bFirst = false;
				}
			}
		}
	}
	return s_result;
}

void Table::substrCell(const MyString& s_cmd) {
	MyString s_expr = "=SUBSTR(" + s_cmd.substr(7) + ")";
	std::cout << "SUBSTR = " << evaluateFormula(s_expr, static_cast<const Table*>(this)) << "\n";
}

void Table::createTable(const MyString& s_cmd) {
	Vector<MyString> tokens = s_cmd.split(' ');
	if (tokens.getSize() < 2U) {
		std::cerr << "Invalid new table command format.\n";
		return;
	}
	MyString s_configPath = tokens[1];
	std::cout << "Creating new table with config: " << s_configPath << "\n";
}

void Table::saveTable() {
	char buf[256];
	std::cout << "Enter file name to save the table: ";
	std::cin.getline(buf, sizeof(buf));
	if (std::cin.fail()) {
		std::cerr << "Error reading file name.\n";
		return;
	}

	MyString s_fileName = MyString::trim(buf);
	std::ofstream file(s_fileName.c_str());
	if (!file.is_open()) {
		std::cerr << "Error opening file for writing: " << s_fileName.c_str() << "\n";
		return;
	}

	// Iterate over rows.
	for (size_t r = 0; r < llu8totalRows; r++) {
		char rowLabel = 'A' + r;  // row label for the file line.
		file << rowLabel << " {";
		bool first = true;
		// Iterate over columns.
		for (size_t c = 0; c < llu8totalCols; c++) {
			if (p_table[r][c]) {
				if (!first)
					file << ",";

				// Build a normal cell address using "letter" + row number.
				char colLetter = 'A' + c;
				size_t rowNumber = r + 1;

				// Convert rowNumber to string.
				std::string rowNumStr;
				{
					size_t temp = rowNumber;
					// Convert temp to string manually.
					do {
						char digit = '0' + static_cast<char>(temp % 10);
						rowNumStr.insert(rowNumStr.begin(), digit);
						temp /= 10;
					} while (temp > 0);
				}

				// Build the address as a std::string by concatenating the column letter and row number.
				std::string addr;
				addr.push_back(colLetter);
				addr.append(rowNumStr);

				// Convert the std::string address back into MyString.
				MyString s_addr(addr.c_str());

				// Write the cell address and its content.
				file << "[" << s_addr.c_str() << ":" << p_table[r][c]->toString().c_str() << "]";
				first = false;
			}
		}
		file << "}\t\n";
	}
	file.close();
	std::cout << "Table saved to " << s_fileName.c_str() << "\n";
}

void Table::openTable(const MyString& s_cmd) {
	Vector<MyString> tokens = s_cmd.split('_');
	if (tokens.getSize() < 3U) {
		std::cerr << "Invalid open command format.\n";
		return;
	}
	MyString s_tableName = tokens[1];
	MyString s_configPath = tokens[2];
	std::cout << "Opening table: " << s_tableName << " with config: " << s_configPath << "\n";
}

void Table::printTable() const {
	const size_t colWidth = 8;
	const size_t leftMargin = 5;

	for (size_t i = 0; i < leftMargin; i++)
		std::cout << " ";
	for (size_t c = 0; c < llu8totalCols; c++) {
		char buf[16];
		size_t idx = 0;
		size_t num = c + 1;
		while (num > 0) {
			buf[idx++] = '0' + static_cast<char>(num % 10);
			num /= 10;
		}
		// Reverse the number string.
		for (size_t i = 0; i < idx / 2; i++) {
			char temp = buf[i];
			buf[i] = buf[idx - 1 - i];
			buf[idx - 1 - i] = temp;
		}
		buf[idx] = '\0';
		size_t headerLen = idx;
		size_t padL = (colWidth > headerLen ? (colWidth - headerLen) / 2 : 0);
		size_t padR = (colWidth > headerLen ? (colWidth - headerLen - padL) : 0);

		std::cout << "|" << " ";
		for (size_t i = 0; i < padL; i++) std::cout << " ";
		std::cout << buf;
		for (size_t i = 0; i < padR; i++) std::cout << " ";
		std::cout << " ";
	}
	std::cout << "|\n";

	size_t fullWidth = leftMargin + (llu8totalCols * (colWidth + 3)) + 1;
	for (size_t i = 0; i < fullWidth; i++)
		std::cout << "-";
	std::cout << "\n";

	for (size_t r = 0; r < llu8totalRows; r++) {
		char rowLabel = 'A' + r;
		std::cout << rowLabel;
		for (size_t i = 1; i < leftMargin; i++)
			std::cout << " ";

		for (size_t c = 0; c < llu8totalCols; c++) {
			std::cout << "|" << " ";
			MyString content = "";
			if (p_table[r][c])
				content = p_table[r][c]->toString();
			if (!bautofit && content.getSize() > llu8visibleCellSymbols)
				content = content.substr(0, llu8visibleCellSymbols);
			size_t contentLen = content.getSize();
			size_t padTotal = (colWidth > contentLen ? colWidth - contentLen : 0);
			size_t padL = 0, padR = 0;
			if (s_initialAlignment == left) {
				padR = padTotal;
			}
			else if (s_initialAlignment == center) {
				padL = padTotal / 2;
				padR = padTotal - padL;
			}
			else if (s_initialAlignment == right) {
				padL = padTotal;
			}
			for (size_t i = 0; i < padL; i++) std::cout << " ";
			std::cout << content.c_str();
			for (size_t i = 0; i < padR; i++) std::cout << " ";
			std::cout << " ";
		}
		std::cout << "|\n";

		for (size_t i = 0; i < fullWidth; i++)
			std::cout << "-";
		std::cout << "\n";
	}
}

void Table::run() {
	char cmd[256];
	while (true) {
		std::cout << "> ";
		if (std::cin.getline(cmd, sizeof(cmd))) {
			MyString s_cmd = MyString::trim(cmd);
			Vector<MyString> tokens = s_cmd.split(' ');
			if (s_cmd == "exit")
				break;
			else if (tokens.getSize() >= 2 && tokens[0].getSize() > 0 &&
				(tokens[0][0] >= 'A' && tokens[0][0] <= 'Z')) {
				MyString s_second = tokens[1];
				if (s_second == "insert")
					insertValue(s_cmd);
				else if (s_second == "delete")
					deleteCell(s_cmd);
				else if (s_second == "reference")
					referenceCell(s_cmd);
				else if (s_second[0] == '=') {
					MyString s_formula = MyString::join(tokens, ' ', 1);
					MyString s_eval = evaluateFormula(s_formula, static_cast<const Table*>(this));
					MyString s_result;
					if (s_formula.startsWith("=SUM") || s_formula.startsWith("=AVERAGE") ||
						s_formula.startsWith("=MIN") || s_formula.startsWith("=MAX")) {
						bool b_convertible = true;
						double d_val = 0.0;
						try {
							d_val = stod(s_eval);
						}
						catch (...) {
							b_convertible = false;
						}
						if (b_convertible) {
							int i_val = (int)d_val;
							if (fabs(d_val - i_val) < 1e-6)
								s_result = MyString(toString(i_val).c_str());
							else
								s_result = MyString(toString(d_val).c_str());
						}
						else {
							s_result = s_eval;
						}
					}
					else {
						s_result = "\"" + s_eval + "\"";
					}
					MyString s_newCmd = tokens[0] + " insert " + s_result;
					insertValue(s_newCmd);
				}
				else
					std::cout << "Unknown command: " << s_cmd << "\n";
			}
			else if (s_cmd.startsWith("open") || s_cmd.startsWith("new") ||
				s_cmd.startsWith("SUM") || s_cmd.startsWith("AVERAGE") ||
				s_cmd.startsWith("MIN") || s_cmd.startsWith("MAX") ||
				s_cmd.startsWith("CONCAT") || s_cmd.startsWith("SUBSTR") ||
				s_cmd.startsWith("LEN")) {
				if (s_cmd.startsWith("open"))
					openTable(s_cmd);
				else if (s_cmd.startsWith("new"))
					createTable(s_cmd);
				else if (s_cmd.startsWith("SUM"))
					sumCells(s_cmd);
				else if (s_cmd.startsWith("AVERAGE"))
					averageCells(s_cmd);
				else if (s_cmd.startsWith("MIN"))
					minCells(s_cmd);
				else if (s_cmd.startsWith("MAX"))
					maxCells(s_cmd);
				else if (s_cmd.startsWith("CONCAT"))
					concatCells(s_cmd);
				else if (s_cmd.startsWith("SUBSTR"))
					substrCell(s_cmd);
				else if (s_cmd.startsWith("LEN"))
					len(tokens, getInstance());
			}
			else
				std::cout << "Unknown command: " << s_cmd << "\n";
			if (bclearConsoleAfterCommand)
				std::cout << "\033[2J\033[H";
			printTable();
		}
		else {
			std::cerr << "Error reading command.\n";
		}
	}
	saveTable();
	std::cout << "Table saved. Exiting.\n";
}

