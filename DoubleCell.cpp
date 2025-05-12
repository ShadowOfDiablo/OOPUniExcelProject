#include "Cell.h"
#include "DoubleCell.h"
DoubleCell::DoubleCell()
{
	value = 0.0;
}
DoubleCell::DoubleCell(double value)
{
	this->value = value;
};
Cell::Type DoubleCell::getType() const
{
	return Type::DOUBLE;
}
double DoubleCell::getValue() const
{
	return value; 
}
void DoubleCell::setValue(double newValue)
{
	value = newValue;
}