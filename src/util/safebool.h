#pragma once

#include <cstdint>

class SafeBool
// Unrelated to the "safe bool idiom"
// Means of having an uninitialized bool type that doesn't default to true or false
{
public:
	SafeBool();
	SafeBool(const bool &copy);

	bool operator==(const bool &comparand) const;
	bool operator==(const SafeBool &comparand) const;
	void operator=(const bool &operand);

	operator bool() const;

private:
	bool Value() const;

	enum class VALUE : uint8_t { INDETERMINATE, FALSE, TRUE } value;
};
