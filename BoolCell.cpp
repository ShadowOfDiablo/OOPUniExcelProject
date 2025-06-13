#include "BoolCell.h"
BoolCell::BoolCell()
{
	this->b_value = false;
}
BoolCell::BoolCell(bool value)
{
	this->b_value = value;
};
Cell::Type BoolCell::getType() const 
{ 
	return Type::BOOL; 
}
bool BoolCell::getValue() const 
{ 
	return b_value;
}
void BoolCell::setValue(bool newValue) 
{ 
    b_value = newValue;
}
MyString BoolCell::toString() const {
    char u8_buffer[6];
    u8_buffer[0U] = b_value ? 't' : 'f';
    u8_buffer[1U] = b_value ? 'r' : 'a';
    u8_buffer[2U] = b_value ? 'u' : 'l';
    u8_buffer[3U] = b_value ? 'e' : 's';
    u8_buffer[4U] = '\0';

    return MyString(u8_buffer);
}

Cell* BoolCell::clone() const {
	return new BoolCell(b_value);
}