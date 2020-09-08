#include "csv.h"

#include <string>
#include <cmath>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

#include "x86.h"
#include "decode.h"

const int OPERAND_COLUMNS_LENGTH = 8; 

enum CSV_COLUMNS
{
	PF = 0, // -127 | (value)
        _0F = 1, // -127 | 1
        PO = 2, // (value)
        SO = 3, // -127 | (value)

        // flds 
        R = 4, // -127 | 1
        W = 5, // -127 | 0 | 1
        S = 6, // -127 | 0 | 1
        D = 7, // -127 | 0 | 1
        TTTN = 8, // -127 | 0 - 15
        SR = 9, // -127 | 0 - 3 
        SRE = 10, // -127 | 0 - 7
        MF = 11, // -127 | 0 - 3

        O = 12, // -127 | 0 - 8
        PROC = 13, // (value)
        ST = 14, // -127 | 1 - 3
        M = 15, // -127 | 1 - 4
        RL = 16, // -127 | 1 - 3
        X = 17, // -127 | 0 - 3 (LOCK, s, p, P)

        MNEMONIC = 18, // -127 | (value)
        OP1METHOD = 19, 
        OP1TYPE = 20,   
        OP2METHOD = 21, 
        OP2TYPE = 22,   
        OP3METHOD = 23, 
        OP3TYPE = 24,   
        OP4METHOD = 25, 
        OP4TYPE = 26,   

        IEXT = 27, // -127 | 1 - 9
	GRP1 = 28, // (value)
        GRP2 = 29, // (value)
        GRP3 = 30, // (value)
        TESTED_F = 31, // -127 | 0 - 7
        MODIF_F = 32, // -127 | 0 - 7
        DEF_F = 33, // -127 | 0 - 7
        UNDEF_F = 34, // -127 | 0 - 7
        F_VALUES = 35, // -127 | 0 - 7
        DESCRIPTION = 36, // (value)

        MNEM_1 = 37, // -127 | (value)
        OP1METHOD_1 = 38, 
        OP1TYPE_1 = 39, 
        OP2METHOD_1 = 40,
        OP2TYPE_1 = 41, 
        OP3METHOD_1 = 42, 
        OP3TYPE_1 = 43, 
        OP4METHOD_1 = 44, 
        OP4TYPE_1 = 45, 

        MNEM_2 = 46, // -127 | (value)
        OP1METHOD_2 = 47, 
        OP1TYPE_2 = 48, 
        OP2METHOD_2 = 49, 
        OP2TYPE_2 = 50, 
        OP3METHOD_2 = 51,
        OP3TYPE_2 = 52,
        OP4METHOD_2 = 53,
        OP4TYPE_2 = 54, 
};

namespace ISet_x86
{

int ParseAsValue(std::string field)
{
	if (field == "") // Empty CSV field for given column
	{
		return INVALID;
	}

	else
	{
		return std::stoi(field, nullptr, 16);
	}
}

int whichOperand = 0; // Track which operand to modify
void ParseOperandColumns(Instruction &instr, std::vector<std::string> values)
{
	whichOperand++;
}

int16_t ParseFlags(std::string flags)
{
	int8_t ret = 0;

	if (flags == "")
	{
		return INVALID;
	}

	else
	{
		for (int i = 7; i >= 0; i--)
		{
			if (flags.at(i) != '.')
			{
				ret += std::pow(2, i);
			}
		}
	}

	return ret;
};

Instruction ParseCSVLine(std::string line)
{
	Instruction instr {};
	std::vector<std::string> csvValues {};

	// Convert each delimited value into an array entry
	std::string tempValue {};
	for (int i = 0; i < line.length(); i++)
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

	int column = 0;
	while (column < csvValues.size())
	{
		std::string field = csvValues.at(column);

		switch (column)
		{
			case CSV_COLUMNS::PF:
				instr.encoded.opcode.mandatoryPrefix = ParseAsValue(csvValues.at(column));
				break;
			case CSV_COLUMNS::_0F:
				instr.encoded.opcode.twoByte = (field != "");
				break;
			case CSV_COLUMNS::PO:
				instr.encoded.opcode.primary = ParseAsValue(field);
				break;
			case CSV_COLUMNS::SO:
				instr.encoded.opcode.secondary = ParseAsValue(field);
				break;

			// Flds
			case CSV_COLUMNS::R:
				instr.encoded.opcode.fields.regEncoded = (field != "" && field != "0");
				break;
			case CSV_COLUMNS::W:
				instr.encoded.opcode.fields.operandSize = (field != "" && field != "0");
				break;
			case CSV_COLUMNS::S:
				instr.encoded.opcode.fields.signExtend = (field != "" && field != "0");
				break;
			case CSV_COLUMNS::D:
				instr.encoded.opcode.fields.direction = (field != "" && field != "0");
				break;
			case CSV_COLUMNS::TTTN:
				instr.encoded.opcode.fields.conditionals = ParseAsValue(field);
				break;
			case CSV_COLUMNS::SR:
			case CSV_COLUMNS::SRE:
				instr.encoded.opcode.fields.segmentRegisterSpecifier = ParseAsValue(field);
				break;
			case CSV_COLUMNS::MF:
				instr.encoded.opcode.fields.memoryFormat = ParseAsValue(field);
				break; 

			case CSV_COLUMNS::O:
				instr.encoded.opcode.extension = ParseAsValue(field);
				break;
			case CSV_COLUMNS::PROC:
				instr.attrib.intrinsic.firstAppearance = field;
				break;
			case CSV_COLUMNS::ST:
				instr.attrib.intrinsic.documentationStatus = ParseAsValue(field);
				break;	
			case CSV_COLUMNS::M:
				instr.attrib.intrinsic.operationMode = ParseAsValue(field);
				break;
			case CSV_COLUMNS::RL:
				instr.attrib.intrinsic.ringLevel = ParseAsValue(field);
				break;
			case CSV_COLUMNS::X:
				instr.attrib.intrinsic.lockAndFPU = ParseAsValue(field);
				break;

			case MNEMONIC:
			case MNEM_1:
			case MNEM_2:
			{
				auto iter = csvValues.begin() + column;
				std::vector<std::string> subvec {iter, iter + OPERAND_COLUMNS_LENGTH};
				ParseOperandColumns(instr, subvec);
				column += OPERAND_COLUMNS_LENGTH;
				break;
			}
			case IEXT:
				instr.attrib.intrinsic.iext = ParseAsValue(field);
				break;
			case GRP1:
				instr.attrib.intrinsic.group1 = field;
				break;
			case GRP2:
				instr.attrib.intrinsic.group2 = field;
				break;
			case GRP3:
				instr.attrib.intrinsic.group3 = field;
				break;
			case TESTED_F:
				instr.attrib.intrinsic.testedFlags = ParseFlags(field);
				break;
			case MODIF_F:
				instr.attrib.intrinsic.modifiedFlags = ParseFlags(field);
				break;
			case DEF_F:
				instr.attrib.intrinsic.definedFlags = ParseFlags(field);
				break;
			case UNDEF_F:
				instr.attrib.intrinsic.undefinedFlags = ParseFlags(field);
				break;
			case F_VALUES:
				instr.attrib.intrinsic.flagValues = ParseFlags(field);
				break;
			case DESCRIPTION:
				instr.attrib.intrinsic.description = field;
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

	csvLines.erase(csvLines.begin());

	std::vector<Instruction> instrs {};
	
	for (auto line : csvLines)
	{
		instrs.push_back(ParseCSVLine(line));
	}
}

};
