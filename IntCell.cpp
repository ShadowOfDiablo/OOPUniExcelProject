#include "IntCell.h"
IntCell::IntCell()
{
    u32_value = 0;
}
IntCell::IntCell(int u32_value)
{
	this->u32_value = u32_value;
};
Cell::Type IntCell::getType() const
{
	return Type::INT;
}
int IntCell::getValue() const 
{
	return u32_value;
}
void IntCell::setValue(int newValue)
{ 
    u32_value = newValue;
}

MyString IntCell::toString() const {
    char u8_buffer[20];
    size_t u32_index = 0U;
    unsigned u32_temp = u32_value;

    if (u32_temp == 0U) {
        u8_buffer[u32_index++] = '0';
    }
    else {
        while (u32_temp > 0U) {
            u8_buffer[u32_index++] = '0' + (u32_temp % 10U);
            u32_temp /= 10U;
        }

        for (size_t u32_i = 0U; u32_i < u32_index / 2U; ++u32_i) {
            char u8_temp = u8_buffer[u32_i];
            u8_buffer[u32_i] = u8_buffer[u32_index - 1U - u32_i];
            u8_buffer[u32_index - 1U - u32_i] = u8_temp;
        }
    }

    u8_buffer[u32_index] = '\0';
    return MyString(u8_buffer);
}
Cell* IntCell::clone() const {
    return new IntCell(u32_value);
}

