#include <stdexcept>

#include "safebool.h"

SafeBool::SafeBool() : value(VALUE::INDETERMINATE) {}
	
SafeBool::SafeBool(const bool &operand) : value(operand ? VALUE::TRUE : VALUE::FALSE) {}

bool SafeBool::operator==(const bool &comparand) const
{
	return comparand == Value();
}
	
bool SafeBool::operator==(const SafeBool &comparand) const
{
	return comparand == Value();
}

void SafeBool::operator=(const bool &operand) 
{
	value = operand ? VALUE::TRUE : VALUE::FALSE;
}

SafeBool::operator bool() const 
{ 
	return Value();
}
	
bool SafeBool::Value() const
{
	if (value == VALUE::INDETERMINATE)
	{
		throw std::logic_error("Attempt to read value of an indeterminate SafeBool instance");
	}
	
	return value == VALUE::TRUE;
}
