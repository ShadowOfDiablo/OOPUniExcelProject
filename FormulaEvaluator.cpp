#include "FormulaEvaluator.h"
#include "MyString.h"
#include <stdexcept>
#include <cstring>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include "Cell.h"
#include "Table.h"
class Cell;
class Table;

bool isFormula(const MyString& c_content) {
    return c_content.getSize() > 0 && c_content[0] == '=';
}

double parseLiteral(const MyString& c_token) {
    bool bDecimalFound = false;
    for (size_t i = 0; i < c_token.getSize(); i++) {
        char c = c_token[i];
        if ((c < '0' || c > '9') && c != '.' && c != '-') {
            throw std::invalid_argument("Invalid numeric literal");
        }
        if (c == '.') {
            if (bDecimalFound)
                throw std::invalid_argument("Multiple decimals in number");
            bDecimalFound = true;
        }
    }
    double dResult = 0.0;
    double dFraction = 0.0;
    double dDivisor = 10.0;
    bool bNegative = false;
    size_t i = 0;
    if (c_token[0] == '-') {
        bNegative = true;
        i++;
    }
    for (; i < c_token.getSize(); i++) {
        char c = c_token[i];
        if (c == '.') {
            i++;
            break;
        }
        dResult = dResult * 10.0 + (c - '0');
    }
    for (; i < c_token.getSize(); i++) {
        dFraction += (c_token[i] - '0') / dDivisor;
        dDivisor *= 10.0;
    }
    double dTotal = dResult + dFraction;
    return bNegative ? -dTotal : dTotal;
}

void parseCellRef(const MyString& c_ref, size_t& row, size_t& col) {
    col = c_ref[0] - 'A';
    row = stoul(c_ref.substr(1)) - 1;
}

MyString resolveCell(const MyString& c_ref, const Table* p_table) {
    size_t row, col;
    parseCellRef(c_ref, row, col);
    Cell* p_cell = p_table->getCell(row, col);
    return p_cell ? p_cell->toString() : "";
}

Vector<MyString> extractArgs(const MyString& c_funcCall) {
    Vector<MyString> vArgs;
    size_t start = c_funcCall.find('(');
    size_t end = c_funcCall.find(')');
    if (start == MyString::npos || end == MyString::npos || end <= start + 1U)
        return vArgs;
    MyString c_raw = c_funcCall.substr(start + 1U, end - start - 1U);
    size_t begin = 0;
    for (size_t i = 0; i <= c_raw.getSize(); i++) {
        if (i == c_raw.getSize() || c_raw[i] == ',') {
            vArgs.push_back(MyString::trim(c_raw.substr(begin, i - begin)));
            begin = i + 1U;
        }
    }
    return vArgs;
}
bool bAllDigits(const MyString& s_num) {
    for (unsigned int u32_i = 0; u32_i < s_num.getSize(); u32_i++) {
        char c_chr = s_num[u32_i];
        if (c_chr < '0' || c_chr > '9')
            return false;
    }
    return true;
}
double sum(const Vector<MyString>& v_args, const Table* p_table) {
    double d_total = 0.0;
    Vector<MyString> v_tokens;
    if (v_args.getSize() == 1 && v_args[0].startsWith("SUM(") && v_args[0].endsWith(")")) {
        MyString s_inner = v_args[0].substr(4, v_args[0].getSize() - 5);
        v_tokens = s_inner.split(',');
    }
    else {
        v_tokens = v_args;
    }
    for (unsigned int u32_i = 0; u32_i < v_tokens.getSize(); u32_i++) {
        MyString s_arg = MyString::trim(v_tokens[u32_i]);
        if (s_arg == "")
            continue;
        if (s_arg.find(':') != MyString::npos) {
            size_t u32_pos = s_arg.find(':');
            MyString s_start = MyString::trim(s_arg.substr(0, u32_pos));
            MyString s_end = MyString::trim(s_arg.substr(u32_pos + 1));
            if (s_start.getSize() < 2 || s_end.getSize() < 2)
                continue;
            char c_rowS = s_start[0];
            char c_rowE = s_end[0];
            unsigned int u32_rowS = c_rowS - 'A';
            unsigned int u32_rowE = c_rowE - 'A';
            MyString s_colStart = s_start.substr(1);
            MyString s_colEnd = s_end.substr(1);
            if (!bAllDigits(s_colStart) || !bAllDigits(s_colEnd))
                continue;
            unsigned int u32_colS = (unsigned int)stoi(s_colStart);
            unsigned int u32_colE = (unsigned int)stoi(s_colEnd);
            if (u32_colS > 0)
                u32_colS--;
            if (u32_colE > 0)
                u32_colE--;
            if (u32_rowS > u32_rowE || u32_colS > u32_colE)
                continue;
            for (unsigned int u32_r = u32_rowS; u32_r <= u32_rowE; u32_r++) {
                for (unsigned int u32_c = u32_colS; u32_c <= u32_colE; u32_c++) {
                    Cell* p_cell = p_table->getCell(u32_r, u32_c);
                    if (p_cell) {
                        MyString s_val = p_cell->toString();
                        try {
                            d_total += stod(s_val);
                        }
                        catch (...) {}
                    }
                }
            }
        }
        else {
            bool b_ref = false;
            if (s_arg.getSize() >= 2 && (s_arg[0] >= 'A' && s_arg[0] <= 'Z')) {
                b_ref = true;
                for (unsigned int u32_j = 1; u32_j < s_arg.getSize(); u32_j++) {
                    char c_chr = s_arg[u32_j];
                    if (c_chr < '0' || c_chr > '9') { b_ref = false; break; }
                }
            }
            if (b_ref) {
                unsigned int u32_row = s_arg[0] - 'A';
                MyString s_col = s_arg.substr(1);
                if (!bAllDigits(s_col))
                    continue;
                unsigned int u32_col = (unsigned int)stoi(s_col);
                if (u32_col > 0)
                    u32_col--;
                Cell* p_cell = p_table->getCell(u32_row, u32_col);
                if (p_cell) {
                    MyString s_val = p_cell->toString();
                    try {
                        d_total += stod(s_val);
                    }
                    catch (...) {}
                }
            }
            else {
                try {
                    d_total += stod(s_arg);
                }
                catch (...) {}
            }
        }
    }
    return d_total;
}

double average(const Vector<MyString>& tokens, const Table* p_table) {
    double dTotal = 0.0;
    int count = 0;
    for (size_t i = 0; i < tokens.getSize(); i++) {
        const MyString& token = tokens[i];
        try {
            size_t colonPos = token.find(':');
            if (colonPos != MyString::npos) {
                MyString cStart = token.substr(0, colonPos);
                MyString cEnd = token.substr(colonPos + 1U);
                size_t startRow, startCol, endRow, endCol;
                parseCellRef(cStart, startRow, startCol);
                parseCellRef(cEnd, endRow, endCol);
                for (size_t r = startRow; r <= endRow; r++) {
                    for (size_t c = startCol; c <= endCol; c++) {
                        Cell* p_cell = p_table->getCell(r, c);
                        if (p_cell) {
                            MyString cVal = p_cell->toString();
                            MyString evaluated = isFormula(cVal) ? evaluateFormula(cVal, p_table) : cVal;
                            dTotal += parseLiteral(evaluated);
                            count++;
                        }
                    }
                }
            }
            else if (token[0] >= 'A' && token[0] <= 'Z') {
                MyString result = resolveCell(token, p_table);
                dTotal += parseLiteral(result);
                count++;
            }
            else {
                dTotal += parseLiteral(token);
                count++;
            }
        }
        catch (...) {
            continue;
        }
    }
    return count > 0 ? (dTotal / count) : 0.0;
}

double min(const Vector<MyString>& tokens, const Table* p_table) {
    if (tokens.getSize() != 1U) {
        throw std::invalid_argument("MIN requires exactly one range");
    }
    const MyString& range = tokens[0];
    size_t colonPos = range.find(':');
    if (colonPos == MyString::npos) {
        throw std::invalid_argument("MIN requires a range");
    }
    MyString cStart = range.substr(0, colonPos);
    MyString cEnd = range.substr(colonPos + 1U);
    size_t startRow, startCol, endRow, endCol;
    parseCellRef(cStart, startRow, startCol);
    parseCellRef(cEnd, endRow, endCol);
    double dMin = 1e9;
    for (size_t r = startRow; r <= endRow; r++) {
        for (size_t c = startCol; c <= endCol; c++) {
            Cell* p_cell = p_table->getCell(r, c);
            if (p_cell) {
                MyString cVal = p_cell->toString();
                MyString evaluated = isFormula(cVal) ? evaluateFormula(cVal, p_table) : cVal;
                double d = parseLiteral(evaluated);
                if (d < dMin)
                    dMin = d;
            }
        }
    }
    return dMin;
}

double len(const Vector<MyString>& tokens, const Table* p_table) {
    if (tokens.getSize() != 1U)
        throw std::invalid_argument("LEN requires one argument");
    const MyString& arg = tokens[0];
    if (arg.find(':') != MyString::npos)
        throw std::invalid_argument("LEN cannot be used on ranges");
    if (arg[0] >= 'A' && arg[0] <= 'Z') {
        size_t row, col;
        parseCellRef(arg, row, col);
        Cell* p_cell = p_table->getCell(row, col);
        return p_cell ? p_cell->toString().getSize() : 0.0;
    }
    else if (arg[0] == '"' && arg[arg.getSize() - 1] == '"') {
        return arg.substr(1, arg.getSize() - 2).getSize();
    }
    return arg.getSize();
}

MyString concat(const Vector<MyString>& tokens, const Table* p_table) {
    if (tokens.getSize() != 2U)
        throw std::invalid_argument("CONCAT requires two arguments");
    const MyString& rangeToken = tokens[0];
    const MyString& delimiterToken = tokens[1];
    MyString delimiter = delimiterToken;
    if (delimiterToken.getSize() >= 2U &&
        delimiterToken[0] == '"' &&
        delimiterToken[delimiterToken.getSize() - 1] == '"') {
        delimiter = delimiterToken.substr(1U, delimiterToken.getSize() - 2U);
    }
    size_t colonPos = rangeToken.find(':');
    if (colonPos == MyString::npos)
        throw std::invalid_argument("First argument to CONCAT must be a range");
    MyString cStart = rangeToken.substr(0, colonPos);
    MyString cEnd = rangeToken.substr(colonPos + 1U);
    size_t startRow, startCol, endRow, endCol;
    parseCellRef(cStart, startRow, startCol);
    parseCellRef(cEnd, endRow, endCol);
    MyString result = "";
    bool first = true;
    for (size_t r = startRow; r <= endRow; r++) {
        for (size_t c = startCol; c <= endCol; c++) {
            Cell* p_cell = p_table->getCell(r, c);
            if (p_cell) {
                MyString val = p_cell->toString();
                if (val.getSize() == 0U)
                    continue;
                if (!first) {
                    result += delimiter;
                }
                result += val;
                first = false;
            }
        }
    }
    return result;
}

MyString toString(double d_value) {
    char c_buffer[64];
    std::snprintf(c_buffer, sizeof(c_buffer), "%.10g", d_value);
    return MyString(c_buffer);
}

MyString evaluateFormula(const MyString& c_expr, const Table* p_table) {
    if (c_expr.getSize() <= 1U || c_expr[0] != '=') {
        return c_expr;
    }
    MyString c_content = c_expr.substr(1U);
    Vector<MyString> args = extractArgs(c_content);
    if (c_content.startsWith("SUM(")) {
        return toString(sum(args, p_table));
    }
    else if (c_content.startsWith("AVERAGE(")) {
        return toString(average(args, p_table));
    }
    else if (c_content.startsWith("MIN(")) {
        return toString(min(args, p_table));
    }
    else if (c_content.startsWith("LEN(")) {
        return toString(len(args, p_table));
    }
    else if (c_content.startsWith("CONCAT(")) {
        return concat(args, p_table);
    }
    else if (c_content.startsWith("SUBSTR(")) {
        if (args.getSize() != 3U)
            return "ERROR: SUBSTR requires 3 args";
        MyString source = args[0];
        int start = stoi(args[1]);
        int count = stoi(args[2]);
        if (source[0] >= 'A' && source[0] <= 'Z') {
            source = resolveCell(source, p_table);
        }
        else if (source[0] == '"' && source[source.getSize() - 1] == '"') {
            source = source.substr(1U, source.getSize() - 2U);
        }
        if (start < 0 || count < 0 || static_cast<size_t>(start) >= source.getSize()) {
            return "";
        }
        return source.substr(static_cast<size_t>(start), static_cast<size_t>(count));
    }
    return "INVALID";
}

int stoi(const MyString& c_str) {
    int u8_value = 0;
    bool b_negative = false;
    size_t start = 0;
    if (c_str[0] == '-') {
        b_negative = true;
        start = 1;
    }
    for (size_t i = start; i < c_str.getSize(); i++) {
        char c = c_str[i];
        if (c < '0' || c > '9')
            throw std::invalid_argument("Invalid character in stoi");
        u8_value = u8_value * 10 + (c - '0');
    }
    return b_negative ? -u8_value : u8_value;
}

unsigned stoul(MyString c_str) {
    unsigned value = 0U;
    for (size_t i = c_str.getSize(); i > 0; --i) {
        char c = c_str[i - 1];
        if (c < '0' || c > '9')
            throw std::invalid_argument("Invalid string for unsigned long conversion");
        value = value * 10U + (c - '0');
    }
    return value;
}

double stod(const MyString& c_str) {
    double d_value = 0.0;
    double d_fraction = 0.0;
    bool negative = false;
    bool fraction = false;
    double divisor = 10.0;
    size_t i = 0;
    if (c_str[0] == '-') {
        negative = true;
        i = 1;
    }
    for (; i < c_str.getSize(); i++) {
        char c = c_str[i];
        if (c == '.') {
            if (fraction)
                throw std::invalid_argument("Multiple decimals in stod");
            fraction = true;
            continue;
        }
        if (c < '0' || c > '9')
            throw std::invalid_argument("Invalid character in stod");
        if (!fraction) {
            d_value = d_value * 10.0 + (c - '0');
        }
        else {
            d_fraction += (c - '0') / divisor;
            divisor *= 10.0;
        }
    }
    double result = d_value + d_fraction;
    return negative ? -result : result;
}
