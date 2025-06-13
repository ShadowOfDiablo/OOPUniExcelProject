#include "Cell.h"
#include "DoubleCell.h"
DoubleCell::DoubleCell()
{
    f64_value = 0.0;
}
DoubleCell::DoubleCell(double value)
{
	this->f64_value = value;
};
Cell::Type DoubleCell::getType() const
{
	return Type::DOUBLE;
}
double DoubleCell::getValue() const
{
	return f64_value;
}
void DoubleCell::setValue(double newValue)
{
    f64_value = newValue;
}
MyString DoubleCell::toString() const {
    char u8_buffer[50];
    size_t u32_index = 0U;
    long long unsigned u64_integer_part = static_cast<long long unsigned>(f64_value);
    double f64_fractional_part = f64_value - u64_integer_part;

    while (u64_integer_part > 0U) {
        u8_buffer[u32_index++] = '0' + (u64_integer_part % 10U);
        u64_integer_part /= 10U;
    }

    for (size_t u32_i = 0U; u32_i < u32_index / 2U; ++u32_i) {
        char u8_temp = u8_buffer[u32_i];
        u8_buffer[u32_i] = u8_buffer[u32_index - 1U - u32_i];
        u8_buffer[u32_index - 1U - u32_i] = u8_temp;
    }

    u8_buffer[u32_index++] = '.';
    for (size_t u32_i = 0U; u32_i < 2U; ++u32_i) {
        f64_fractional_part *= 10U;
        u8_buffer[u32_index++] = '0' + static_cast<int>(f64_fractional_part);
        f64_fractional_part -= static_cast<int>(f64_fractional_part);
    }

    u8_buffer[u32_index] = '\0';
    return MyString(u8_buffer);
}

Cell* DoubleCell::clone() const {
    return new DoubleCell(f64_value);
}

