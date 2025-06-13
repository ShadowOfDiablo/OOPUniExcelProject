#include "FormulaCell.h"
#include "Table.h"            
#include "FormulaEvaluator.h"

FormulaCell::FormulaCell(const MyString& formula)
    : c_formula(formula) {
}

Cell* FormulaCell::clone() const {
    return new FormulaCell(c_formula);
}

MyString FormulaCell::toString() const {
    try {
        return evaluateFormula(c_formula, Table::getInstance());
    }
    catch (...) {
        return "#VALUE!";
    }
}

Cell::Type FormulaCell::getType() const {
    return Type::FORMULA;
}
