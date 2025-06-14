#include "FormulaEvaluator.h"
#include "MyString.h"
#include <stdexcept>
#include <cstring>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include "Cell.h"
#include "Table.h"
#include "Table.cpp"
class Cell;
bool isFormula(const MyString& c_content) {
    return c_content.getSize() > 0 && c_content[0] == '=';
}
bool bIsNumber(const MyString& s) {
    if (s.getSize() == 0) {
        return false;
    }

    unsigned int u32_start = 0;
    if (s[0] == '+' || s[0] == '-') {
        u32_start = 1;
        if (s.getSize() == 1) {
            return false;
        }
    }

    bool b_hasDigit = false;
    bool b_hasDot = false;

    for (unsigned int u32_i = u32_start; u32_i < s.getSize(); u32_i++) {
        char c_chr = s[u32_i];
        if (c_chr >= '0' && c_chr <= '9') {
            b_hasDigit = true;
        }
        else if (c_chr == '.') {
            if (b_hasDot) {
                return false; // Multiple decimals
            }
            b_hasDot = true;
        }
        else {
            return false; // Invalid character
        }
    }

    return b_hasDigit; // Must have at least one digit
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

double sum(const Vector<MyString>& tokens, const Table* p_table) {
    double d_total = 0.0;
    Vector<MyString> v_tokens;
    if (tokens.getSize() == 1 && tokens[0].startsWith("SUM(") && tokens[0].endsWith(")")) {
        MyString s_inner = tokens[0].substr(4, tokens[0].getSize() - 5);
        v_tokens = s_inner.split(',');
    }
    else {
        v_tokens = tokens;
    }

    for (unsigned int u32 = 0; u32 < v_tokens.getSize(); u32++) {
        MyString c_arg = MyString::trim(v_tokens[u32]);
        if (c_arg == "")
            continue;
        if (c_arg.find(':') != MyString::npos) {
            size_t pos = c_arg.find(':');
            MyString c_start = MyString::trim(c_arg.substr(0, pos));
            MyString c_end = MyString::trim(c_arg.substr(pos + 1));
            // Instead of extracting the letter manually, use parseCellRef:
            size_t rowStart, colStart, rowEnd, colEnd;
            try {
                parseCellRef(c_start, rowStart, colStart);
                parseCellRef(c_end, rowEnd, colEnd);
            }
            catch (...) {
                continue;
            }
            if (rowStart > rowEnd || colStart > colEnd)
                continue;
            for (size_t r = rowStart; r <= rowEnd; r++) {
                for (size_t c = colStart; c <= colEnd; c++) {
                    Cell* p_cell = p_table->getCell(r, c);
                    if (p_cell) {
                        MyString c_val = p_cell->toString();
                        try {
                            d_total += stod(c_val);
                        }
                        catch (...) {}
                    }
                }
            }
        }
        else {
            bool b_isRef = false;
            if (c_arg.getSize() >= 2 && (c_arg[0] >= 'A' && c_arg[0] <= 'Z')) {
                b_isRef = true;
                for (unsigned int j = 1; j < c_arg.getSize(); j++) {
                    char c_chr = c_arg[j];
                    if (c_chr < '0' || c_chr > '9') {
                        b_isRef = false;
                        break;
                    }
                }
            }
            if (b_isRef) {
                size_t row, col;
                try {
                    parseCellRef(c_arg, row, col);
                }
                catch (...) {
                    continue;
                }
                Cell* p_cell = p_table->getCell(row, col);
                if (p_cell) {
                    MyString c_val = p_cell->toString();
                    try {
                        d_total += stod(c_val);
                    }
                    catch (...) {}
                }
            }
            else {
                try {
                    d_total += stod(c_arg);
                }
                catch (...) { continue; }
            }
        }
    }
    return d_total;
}

void parseCellRef(const MyString& c_ref, size_t& row, size_t& col) {
    if (c_ref.getSize() < 2)
        throw std::invalid_argument("Invalid cell reference.");

    row = c_ref[0] - 'A';
    col = stoul(c_ref.substr(1)) - 1;
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
    for (unsigned int i = 0; i < s_num.getSize(); i++) {
        char c_chr = s_num[i];
        if (c_chr < '0' || c_chr > '9')
            return false;
    }
    return true;
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
    if (tokens.getSize() != 1U)
        throw std::invalid_argument("MIN requires exactly one range");

    const MyString& range = tokens[0];
    size_t colonPos = range.find(':');
    if (colonPos == MyString::npos)
        throw std::invalid_argument("MIN requires a range");

    MyString cStart = range.substr(0, colonPos);
    MyString cEnd = range.substr(colonPos + 1U);

    size_t startRow, startCol, endRow, endCol;
    parseCellRef(cStart, startRow, startCol);
    parseCellRef(cEnd, endRow, endCol);

    double dMin = std::numeric_limits<double>::max();
    bool found = false;

    for (size_t r = startRow; r <= endRow; r++) {
        for (size_t c = startCol; c <= endCol; c++) {
            Cell* p_cell = p_table->getCell(r, c);
            if (p_cell) {
                MyString cVal = p_cell->toString();
                MyString evaluated = isFormula(cVal) ? evaluateFormula(cVal, p_table) : cVal;
                // Check if the evaluated string is a valid number before trying to convert.
                if (!bIsNumber(evaluated))
                    continue;
                try {
                    double d = parseLiteral(evaluated);
                    if (!found || d < dMin) {
                        dMin = d;
                        found = true;
                    }
                }
                catch (...) {
                    // Even though we checked bIsNumber, an unexpected error might occur;
                    // in that case, just skip this cell.
                    continue;
                }
            }
        }
    }

    if (!found)
        throw std::invalid_argument("No valid numeric cells found in range for MIN");

    return dMin;
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
    MyString c_start = rangeToken.substr(0, colonPos);
    MyString c_end = rangeToken.substr(colonPos + 1U);
    size_t startRow, startCol, endRow, endCol;
    parseCellRef(c_start, startRow, startCol);
    parseCellRef(c_end, endRow, endCol);
    MyString result = "";
    bool first = true;
    for (size_t r = startRow; r <= endRow; r++) {
        for (size_t c = startCol; c <= endCol; c++) {
            Cell* p_cell = p_table->getCell(r, c);
            if (p_cell) {
                MyString val = p_cell->toString();
                if (val.getSize() == 0U)
                    continue;
                // Only include if all characters are digits.
                if (!bAllDigits(val))
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


MyString toString(double d_value) {
    char c_buffer[64];
    std::snprintf(c_buffer, sizeof(c_buffer), "%.10g", d_value);
    return MyString(c_buffer);
}

MyString evaluateFormula(const MyString& c_expr, const Table* p_table) {
    if (c_expr.getSize() <= 1U || c_expr[0] != '=')
        return c_expr;

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
    else if (c_content.startsWith("MAX(")) {
        if (args.getSize() != 1U)
            return "#VALUE!";
        const MyString& range = args[0];
        size_t colonPos = range.find(':');
        if (colonPos == MyString::npos)
            return "#VALUE!";
        MyString c_start = range.substr(0, colonPos);
        MyString c_end = range.substr(colonPos + 1U);
        size_t startRow, startCol, endRow, endCol;
        parseCellRef(c_start, startRow, startCol);
        parseCellRef(c_end, endRow, endCol);
        double d_max = -1e9;
        for (size_t r = startRow; r <= endRow; r++) {
            for (size_t c = startCol; c <= endCol; c++) {
                Cell* p_cell = p_table->getCell(r, c);
                if (p_cell) {
                    MyString c_val = p_cell->toString();
                    MyString evaluated = isFormula(c_val) ? evaluateFormula(c_val, p_table) : c_val;
                    try {
                        double d = stod(evaluated);
                        if (d > d_max)
                            d_max = d;
                    }
                    catch (...) {
                        continue;
                    }
                }
            }
        }
        return toString(d_max);
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
        if (source[0] >= 'A' && source[0] <= 'Z')
            source = resolveCell(source, p_table);
        else if (source[0] == '"' && source[source.getSize() - 1] == '"')
            source = source.substr(1U, source.getSize() - 2U);
        if (start < 0 || count < 0)
            return "#VALUE!";
        if (static_cast<size_t>(start) >= source.getSize())
            return "";
        return source.substr(static_cast<size_t>(start), static_cast<size_t>(count));
    }
    else if (c_content.startsWith("IF(")) {
        if (args.getSize() != 3U)
            return "#VALUE!";
        MyString condition = args[0];
        // Resolve the condition if it is a cell reference or remove quotes.
        if (condition.getSize() > 0 && condition[0] >= 'A' && condition[0] <= 'Z')
            condition = resolveCell(condition, p_table);
        else if (condition.getSize() >= 2 && condition[0] == '"' && condition[condition.getSize() - 1] == '"')
            condition = condition.substr(1U, condition.getSize() - 2U);

        // Build a lowercase version of the condition.
        MyString cond_lower;
        for (size_t i = 0; i < condition.getSize(); i++) {
            char ch = condition[i];
            if (ch >= 'A' && ch <= 'Z')
                ch = ch + ('a' - 'A');
            cond_lower = cond_lower + ch;
        }

        bool b_cond;
        if (cond_lower == "true")
            b_cond = true;
        else if (cond_lower == "false")
            b_cond = false;
        else
            return "#VALUE!";

        MyString result = b_cond ? args[1] : args[2];
        if (result.getSize() >= 2 && result[0] == '"' && result[result.getSize() - 1] == '"')
            result = result.substr(1U, result.getSize() - 2U);
        return result;
    }

    // Fallback: if c_content is a simple cell reference (e.g., "E5"), resolve it.
    if (c_content.getSize() >= 2 && c_content[0] >= 'A' && c_content[0] <= 'Z') {
        bool isSimpleRef = true;
        for (size_t i = 1; i < c_content.getSize(); i++) {
            char c = c_content[i];
            if (c < '0' || c > '9') {
                isSimpleRef = false;
                break;
            }
        }
        if (isSimpleRef)
            return resolveCell(c_content, p_table);
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
