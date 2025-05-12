#include "IntCell.h"
IntCell::IntCell()
{
	value = 0;
}
IntCell::IntCell(int value)
{
	this->value = value;
};
Cell::Type IntCell::getType() const
{
	return Type::INT;
}
int IntCell::getValue() const 
{
	return value; 
}
void IntCell::setValue(int newValue)
{ 
	value = newValue; 
}