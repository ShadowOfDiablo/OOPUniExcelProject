#include "BoolCell.h"
BoolCell::BoolCell()
{
	this->value = false;
}
BoolCell::BoolCell(bool value)
{
	this->value = value;
};
Cell::Type BoolCell::getType() const 
{ 
	return Type::BOOL; 
}
bool BoolCell::getValue() const 
{ 
	return value; 
}
void BoolCell::setValue(bool newValue) 
{ 
	value = newValue; 
}
