#include "csv.h"

#include <string>
#include <set>
#include <cmath>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

#include "x86.h"
#include "decode.h"

enum CSV_COLUMNS
{
        MNEMONIC = 0,
	PREF = 1,
	TWOBYTE = 2,
	PRI_OPCD = 3,
	SEC_OPCD = 4,
	PLUSR = 5,
	OP_SIZE = 6,
	SIGN_EXT = 7,
	DIRECTION = 8,
	TTTN = 9,
	MEM_FORMAT = 10,
	OPCD_EXT = 11,
	MODE = 12,
	RING = 13,
	LOCK = 14,
	FPUSH = 15,
	FPOP = 16,
	ALIAS = 17,
	PART_ALIAS = 18,
	INSTR_EXT = 19,
	GRP1 = 20,
	GRP2 = 21,
	GRP3 = 22,
	TEST_F = 23,
	MODIF_F = 24,
	DEF_F = 25,
	UNDEF_F = 26,
	F_VALS = 27,
	EXCLUSIVE = 28,
	BRIEF = 29,

	OP1_M = 30,
	OP1_T = 31,
	OP2_M = 32,
	OP2_T = 33,
	OP3_M = 34,
	OP3_T = 35,
	OP4_M = 36,
	OP4_T = 37
};

namespace ISet_x86
{

int ParseAsValue(std::string field, int radix=16)
{
	if (field == "") // Empty CSV field for given column
	{
		return INVALID;
	}

	else
	{
		return std::stoi(field, nullptr, radix);
	}
}

char ParseAsChar(std::string field)
{
	if (field.size() > 0)
	{
		return field.at(0);
	}

	return ' ';
}

bool ParseAsBool(std::string field)
{
	if (field == "" || (field != "1" && field != "0")) // Empty CSV field for given column
	{
		return false;
	}

	// Special cases for 'lock', 'fpush', and 'fpop' fields
	else if (field == "yes" || field == "once")
	{
		return true;
	}

	else
	{
		return static_cast<bool>(ParseAsValue(field));
	}
}

Instruction ParseCSVLine(std::string line)
{
	Instruction instr {};
	std::vector<std::string> csvValues {};

	// Convert each delimited value into an array entry
	std::string tempValue {};
	for (unsigned int i = 0; i < line.length(); i++)
	{
		if (line.at(i) == ',')
		{
			csvValues.push_back(tempValue);
			tempValue = "";
		}

		else
		{
			tempValue += line.at(i);
		}
	}

	unsigned int column = 0;
	while (column < csvValues.size())
	{
		std::string field = csvValues.at(column);

		switch (column)
		{
			case CSV_COLUMNS::MNEMONIC:
				instr.attrib.intrinsic.mnemonic = field;
				break;
			case CSV_COLUMNS::PREF:
				instr.encoded.opcode.mandatoryPrefix = ParseAsValue(csvValues.at(column));
				break;
			case CSV_COLUMNS::TWOBYTE:
				instr.encoded.opcode.twoByte = ParseAsBool(csvValues.at(column));
				break;
			case CSV_COLUMNS::PRI_OPCD:
				instr.encoded.opcode.primary = ParseAsValue(field);
				break;
			case CSV_COLUMNS::SEC_OPCD:
				instr.encoded.opcode.secondary = ParseAsValue(field);
				break;
			case CSV_COLUMNS::PLUSR:
				instr.attrib.intrinsic.plusr = ParseAsBool(field);
				break;
			case CSV_COLUMNS::OP_SIZE:
				instr.encoded.opcode.fields.operandSize = ParseAsBool(field);
				break;
			case CSV_COLUMNS::SIGN_EXT:
				instr.encoded.opcode.fields.signExtend = ParseAsBool(field);
				break;
			case CSV_COLUMNS::DIRECTION:
				instr.encoded.opcode.fields.direction = ParseAsBool(field);
				break;
			case CSV_COLUMNS::TTTN:
				instr.encoded.opcode.fields.conditionals = ParseAsValue(field, 2);
				break;
			case CSV_COLUMNS::MEM_FORMAT:
				instr.encoded.opcode.fields.memoryFormat = ParseAsValue(field, 2);
				break;
			case CSV_COLUMNS::OPCD_EXT:
				instr.encoded.opcode.extension = ParseAsValue(field, 10);
				break;
			case CSV_COLUMNS::MODE:
				instr.attrib.intrinsic.operationMode = ParseAsChar(field);
				break;
			case CSV_COLUMNS::RING:
				instr.attrib.intrinsic.ringLevel = ParseAsChar(field);
				break;
			case CSV_COLUMNS::LOCK:
				instr.attrib.intrinsic.lock = ParseAsBool(field);
				break;
			case CSV_COLUMNS::FPUSH:
				instr.attrib.intrinsic.fpush = ParseAsBool(field);
				break;
			case CSV_COLUMNS::FPOP:
				instr.attrib.intrinsic.fpop = ParseAsBool(field);
				break;
			case CSV_COLUMNS::ALIAS:
				instr.attrib.intrinsic.alias = field;
				break;
			case CSV_COLUMNS::PART_ALIAS:
				// Unused
				break;
			case CSV_COLUMNS::INSTR_EXT:
				instr.attrib.intrinsic.iext = field;
				break;
			case CSV_COLUMNS::GRP1:
				instr.attrib.intrinsic.group1 = field;
				break;
			case CSV_COLUMNS::GRP2:
				instr.attrib.intrinsic.group2 = field;
				break;
			case CSV_COLUMNS::GRP3:
				instr.attrib.intrinsic.group3 = field;
				break;
			case CSV_COLUMNS::TEST_F:
				instr.attrib.intrinsic.testedFlags = field;
				break;
			case CSV_COLUMNS::MODIF_F:
				instr.attrib.intrinsic.modifiedFlags = field;
				break;
			case CSV_COLUMNS::DEF_F:
				instr.attrib.intrinsic.definedFlags = field;
				break;
			case CSV_COLUMNS::UNDEF_F:
				instr.attrib.intrinsic.undefinedFlags = field;
				break;
			case CSV_COLUMNS::F_VALS:
				instr.attrib.intrinsic.flagValues = field;
				break;
			case CSV_COLUMNS::EXCLUSIVE:
				if (field == "32")
				{
                    instr.attrib.intrinsic.x86Exclusive = true;
				}
				else if (field == "64")
				{
                    instr.attrib.intrinsic.x64Exclusive = true;
				}

				break;
			case CSV_COLUMNS::BRIEF:
				instr.attrib.intrinsic.brief = field;
				break;
			case CSV_COLUMNS::OP1_M:
                instr.op1.attrib.intrinsic.addrMethod = static_cast<AddrMethod>(ParseAsValue(field, 10));
				break;
			case CSV_COLUMNS::OP1_T:
                instr.op1.attrib.intrinsic.type = static_cast<OperandType>(ParseAsValue(field, 10));
				break;
			case CSV_COLUMNS::OP2_M:
                instr.op2.attrib.intrinsic.addrMethod = static_cast<AddrMethod>(ParseAsValue(field, 10));
				break;
			case CSV_COLUMNS::OP2_T:
                instr.op2.attrib.intrinsic.type = static_cast<OperandType>(ParseAsValue(field, 10));
				break;
			case CSV_COLUMNS::OP3_M:
                instr.op3.attrib.intrinsic.addrMethod = static_cast<AddrMethod>(ParseAsValue(field, 10));
				break;
			case CSV_COLUMNS::OP3_T:
                instr.op3.attrib.intrinsic.type = static_cast<OperandType>(ParseAsValue(field, 10));
				break;
			case CSV_COLUMNS::OP4_M:
                instr.op4.attrib.intrinsic.addrMethod = static_cast<AddrMethod>(ParseAsValue(field, 10));
				break;
			case CSV_COLUMNS::OP4_T:
                instr.op4.attrib.intrinsic.type = static_cast<OperandType>(ParseAsValue(field, 10));
				break;
			default:
				throw std::logic_error("Missed a column when implementing CSV parsing: " + std::to_string(column));
				break;
		}

		column++;
	}

	return instr;
}

void x86CSVParse(InstructionReference &instrReference)
{
	std::string path = "../data/x86.csv";
	std::ifstream csvFile {path};
	if (!csvFile.is_open())
	{
		throw std::runtime_error("Failed to open x86.csv");
	}

	std::vector<std::string> csvLines {};

	for (std::string line {}; std::getline(csvFile, line); csvLines.push_back(line));

	// Get rid of the label row
	csvLines.erase(csvLines.begin());

	std::vector<Instruction> instrs {};
	
	for (auto line : csvLines)
	{
		auto instr = ParseCSVLine(line);
		if (instrReference.count(instr.encoded.opcode) == 0)
		{
			auto opkey {instr.encoded.opcode};
			instrReference.Emplace(opkey, instr);
		}
	}
}

void threeByteOpcodeCSVParse()
{
	std::string path = "../data/secopcd.csv";
	std::ifstream csvFile {path};
	if (!csvFile.is_open())
	{
		throw std::runtime_error("Failed to open x86.csv");
	}

	std::vector<std::string> csvLines {};

	for (std::string line {}; std::getline(csvFile, line); csvLines.push_back(line));
	// Get rid of the label row
	csvLines.erase(csvLines.begin());

	for (auto line : csvLines)
	{
		std::vector<std::string> csvValues {};
		std::string tempValue {};
		for (unsigned int i = 0; i < line.length(); i++)
		{
			if (line.at(i) == ',')
			{
				csvValues.push_back(tempValue);
				tempValue = "";
			}

			else
			{
				tempValue += line.at(i);
			}
		}	

		bool twoByte = ParseAsBool(csvValues.at(0));
        int16_t primary = ParseAsValue(csvValues.at(1));
        int16_t secondary = ParseAsValue(csvValues.at(2));
		ThreeByteKey tbk = { twoByte, primary };
		threeByteReference[tbk] = secondary;
	}
}

};
