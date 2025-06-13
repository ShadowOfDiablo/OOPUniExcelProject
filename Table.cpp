#define _CRT_SECURE_NO_WARNINGS
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

Table::Table() {
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
	p_table = Vector<Vector<Cell*>>(llu8initialTableRows);
	for (size_t i = 0; i < llu8initialTableRows; i++) {
		Vector<Cell*> tmp(llu8initialTableCols);
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

void Table::validateConfiguration() const {
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
		if (line[0] == '\0')
			continue;
		char rowLabel = line[0];
		size_t row = rowLabel - 'A';
		char* braceStart = std::strchr(line, '{');
		char* braceEnd = std::strchr(line, '}');
		if (!braceStart || !braceEnd)
			continue;
		size_t len = braceEnd - braceStart - 1;
		char content[512];
		std::strncpy(content, braceStart + 1, len);
		content[len] = '\0';
		char* pos = content;
		while (true) {
			char* tokenStart = std::strchr(pos, '[');
			if (!tokenStart)
				break;
			char* tokenEnd = std::strchr(tokenStart, ']');
			if (!tokenEnd)
				break;
			size_t tokenLen = tokenEnd - tokenStart - 1;
			char token[128];
			std::strncpy(token, tokenStart + 1, tokenLen);
			token[tokenLen] = '\0';
			char* colon = std::strchr(token, ':');
			if (colon) {
				*colon = '\0';
				const char* addr = token;
				const char* val = colon + 1;
				if (std::strlen(addr) >= 2) {
					char addrRow = addr[0];
					size_t addrRowInd = addrRow - 'A';
					int colNum = std::atoi(addr + 1);
					size_t addrColInd = (size_t)colNum - 1;
					if (addrRowInd == row) {
						if (std::strlen(val) == 0)
							setCell(addrRowInd, addrColInd, nullptr);
						else {
							MyString s_val(val);
							if (s_val.startsWith("="))
								setCell(addrRowInd, addrColInd, new FormulaCell(s_val));
							else if (s_val.startsWith("\"") && s_val.endsWith("\""))
								setCell(addrRowInd, addrColInd, new MyStringCell(s_val.substr(1, s_val.getSize() - 2)));
							else if (s_val.find('.') != -1)
								setCell(addrRowInd, addrColInd, new DoubleCell(stod(MyString(val))));
							else
								setCell(addrRowInd, addrColInd, new IntCell(stoi(MyString(val))));
						}
					}
				}
			}
			pos = tokenEnd + 1;
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

void Table::run() {
	char cmd[256];
	while (true) {
		std::cout << "> ";
		if (std::cin.getline(cmd, sizeof(cmd))) {
			MyString c_cmd = MyString::trim(cmd);
			Vector<MyString> tokens = c_cmd.split(' ');
			if (c_cmd == "exit")
				break;
			else if (tokens.getSize() >= 2 && tokens[0].getSize() > 0 &&
				(tokens[0][0] >= 'A' && tokens[0][0] <= 'Z')) {
				MyString c_second = tokens[1];
				if (c_second == "insert")
					insertValue(c_cmd);
				else if (c_second == "delete")
					deleteCell(c_cmd);
				else if (c_second == "reference")
					referenceCell(c_cmd);
				else if (c_second[0] == '=') {
					MyString c_formula = MyString::join(tokens, ' ', 1);
					MyString c_eval = evaluateFormula(c_formula, static_cast<const Table*>(this));
					bool b_convertible = true;
					double d_val = 0.0;
					try {
						d_val = stod(c_eval);
					}
					catch (...) {
						b_convertible = false;
					}
					MyString c_result;
					if (b_convertible) {
						int i_val = (int)d_val;
						if (fabs(d_val - i_val) < 1e-6)
							c_result = MyString(toString(i_val).c_str());
						else
							c_result = MyString(toString(d_val).c_str());
					}
					else {
						c_result = c_eval;
					}
					MyString c_newCmd = tokens[0] + " insert " + c_result;
					insertValue(c_newCmd);
				}
				else
					std::cout << "Unknown command: " << c_cmd << "\n";
			}
			else if (c_cmd.startsWith("open") || c_cmd.startsWith("new") ||
				c_cmd.startsWith("SUM") || c_cmd.startsWith("AVERAGE") ||
				c_cmd.startsWith("MIN") || c_cmd.startsWith("MAX") ||
				c_cmd.startsWith("CONCAT") || c_cmd.startsWith("SUBSTR")) {
				if (c_cmd.startsWith("open"))
					openTable(c_cmd);
				else if (c_cmd.startsWith("new"))
					createTable(c_cmd);
				else if (c_cmd.startsWith("SUM"))
					sumCells(c_cmd);
				else if (c_cmd.startsWith("AVERAGE"))
					averageCells(c_cmd);
				else if (c_cmd.startsWith("MIN"))
					minCells(c_cmd);
				else if (c_cmd.startsWith("MAX"))
					maxCells(c_cmd);
				else if (c_cmd.startsWith("CONCAT"))
					concatCells(c_cmd);
				else if (c_cmd.startsWith("SUBSTR"))
					substrCell(c_cmd);
			}
			else
				std::cout << "Unknown command: " << c_cmd << "\n";
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
	insertValue("B1 insert 9");
	insertValue("C1 insert 9");
	insertValue("B2 insert 7");
	insertValue("C2 insert 2");
	insertValue("B3 insert 0");
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
				MyString s_eval = isFormula(s_val) ? evaluateFormula(s_val, static_cast<const Table*>(this)) : s_val;
				double d_tmp = parseLiteral(s_eval);
				if (d_tmp > d_max)
					d_max = d_tmp;
			}
		}
	}
	std::cout << "MAX = " << d_max << "\n";
}

MyString Table::concatCells(const MyString& c_cmd) {
	unsigned int u32_prefix = 8;
	if (c_cmd.getSize() < u32_prefix + 1)
		return "";
	MyString c_inner = c_cmd.substr(u32_prefix, c_cmd.getSize() - u32_prefix - 1);
	Vector<MyString> v_params = c_inner.split(',');
	if (v_params.getSize() != 2)
		return "#VALUE!";
	MyString c_range = MyString::trim(v_params[0]);
	MyString c_delim = MyString::trim(v_params[1]);
	if (c_range.find(':') == -1)
		return "#VALUE!";
	size_t u32_colonPos = c_range.find(':');
	MyString c_start = MyString::trim(c_range.substr(0, u32_colonPos));
	MyString c_end = MyString::trim(c_range.substr(u32_colonPos + 1));
	if (c_start.getSize() < 2 || c_end.getSize() < 2)
		return "#VALUE!";
	char c_rowS = c_start[0];
	char c_rowE = c_end[0];
	unsigned int u32_rowS = c_rowS - 'A';
	unsigned int u32_rowE = c_rowE - 'A';
	MyString c_colStart = c_start.substr(1);
	MyString c_colEnd = c_end.substr(1);
	if (!bAllDigits(c_colStart) || !bAllDigits(c_colEnd))
		return "#VALUE!";
	unsigned int u32_colS = (unsigned int)stoi(c_colStart);
	unsigned int u32_colE = (unsigned int)stoi(c_colEnd);
	if (u32_colS > 0)
		u32_colS--;
	if (u32_colE > 0)
		u32_colE--;
	if (u32_rowS > u32_rowE || u32_colS > u32_colE)
		return "#VALUE!";
	MyString c_result = "";
	bool b_first = true;
	for (unsigned int u32_r = u32_rowS; u32_r <= u32_rowE; u32_r++) {
		for (unsigned int u32_c = u32_colS; u32_c <= u32_colE; u32_c++) {
			Cell* p_cell = getCell(u32_r, u32_c);
			if (p_cell) {
				MyString c_val = p_cell->toString();
				if (c_val != "") {
					if (!b_first)
						c_result = c_result + c_delim;
					c_result = c_result + c_val;
					b_first = false;
				}
			}
		}
	}
	return c_result;
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
	for (size_t r = 0; r < llu8totalRows; r++) {
		char rowLabel = 'A' + r;
		file << rowLabel << " {";
		bool first = true;
		for (size_t c = 0; c < llu8totalCols; c++) {
			if (p_table[r][c]) {
				if (!first)
					file << ",";
				MyString s_addr = MyString() + rowLabel + MyString(toString(c + 1).c_str());
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
	std::cout << "   ";
	for (size_t c = 0; c < llu8totalCols; c++) {
		char buf[16];
		size_t idx = 0;
		size_t num = c + 1;
		while (num > 0) {
			buf[idx++] = '0' + (char)(num % 10);
			num /= 10;
		}
		for (size_t i = 0; i < idx / 2; i++) {
			char temp = buf[i];
			buf[i] = buf[idx - 1 - i];
			buf[idx - 1 - i] = temp;
		}
		buf[idx] = '\0';
		size_t padL = (colWidth > idx ? (colWidth - idx) / 2 : 0);
		size_t padR = colWidth - idx - padL;
		std::cout << std::string(padL, ' ') << buf << std::string(padR, ' ') << "|";
	}
	std::cout << "\n   ";
	for (size_t c = 0; c < llu8totalCols; c++)
		std::cout << std::string(colWidth, '_') << "|";
	std::cout << "\n";
	for (size_t r = 0; r < llu8totalRows; r++) {
		char rowLabel = 'A' + r;
		std::cout << rowLabel << " |";
		for (size_t c = 0; c < llu8totalCols; c++) {
			MyString content = "";
			if (p_table[r][c])
				content = p_table[r][c]->toString();
			if (!bautofit && content.getSize() > llu8visibleCellSymbols)
				content = content.substr(0, llu8visibleCellSymbols);
			size_t contentLen = content.getSize();
			size_t pad = (colWidth > contentLen ? colWidth - contentLen : 0);
			size_t padL = 0, padR = 0;
			if (s_initialAlignment == left)
				padR = pad;
			else if (s_initialAlignment == center) {
				padL = pad / 2;
				padR = pad - padL;
			}
			else if (s_initialAlignment == right)
				padL = pad;
			std::cout << std::string(padL, ' ') << content << std::string(padR, ' ') << "|";
		}
		std::cout << "\n";
	}
}

Alignment stoal(const MyString& s_alignment) {
	if (s_alignment == "left")
		return left;
	if (s_alignment == "center")
		return center;
	if (s_alignment == "right")
		return right;
	throw std::invalid_argument("Invalid alignment value");
}

