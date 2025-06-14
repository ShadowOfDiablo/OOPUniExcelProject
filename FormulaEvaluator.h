// FormulaEvaluator.h
#pragma once

#include "MyString.h"
#include "Vector.hpp"
#include "Table.h"

class Table;

// Conversion functions
unsigned stoul(MyString c_str);
int stoi(const MyString& c_str);
double stod(const MyString& c_str);

bool isFormula(const MyString& c_input);
bool bIsNumber(const MyString& s);
MyString evaluateFormula(const MyString& c_expr, const Table* p_table);
MyString resolveCell(const MyString& c_ref, const Table* p_table);
Vector<MyString> extractArgs(const MyString& c_expr);
bool bAllDigits(const MyString& s_num);
MyString concat(const Vector<MyString>& c_args, const Table* p_table);

double parseLiteral(const MyString& c_token);
double sum(const Vector<MyString>& c_tokens, const Table* p_table);
double average(const Vector<MyString>& c_tokens, const Table* p_table);
double min(const Vector<MyString>& c_tokens, const Table* p_table);
double len(const Vector<MyString>& c_tokens, const Table* p_table);
MyString toString(double d_value);

void parseCellRef(const MyString& c_ref, size_t& u32_row, size_t& u32_col);
