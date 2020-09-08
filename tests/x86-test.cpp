#include <algorithm>
#include <fstream>
#include <cctype>
#include <string>

#include "common-test.h"

#include "../src/arch/arch.h"
#include "../src/format/format.h"

#include "../src/arch/x86/enum.h"
#include "../src/arch/x86/instruction.h"
#include "../src/arch/x86/decode.h"
#include "../src/arch/x86/translate.h"
#include "../src/arch/x86/x86.h"

using namespace ISet_x86;

// To be adjusted whenever I decide on a specific style
const std::vector<std::string> knownCorrectAssembly
{
	"push ebx",
	"sub esp, 0x8",
	"call loc_00000584",
	"add ebx, 0x29d7b",
	"mov eax, DWORD PTR [ebx+0x170]",
	"test eax, eax"
	"je loc_0000001e",
	"call loc_00000534",

	"loc_0000001e:"
	"add esp, 0x8",
	"pop ebx",
	"ret"
};

std::vector<std::string> assembly;
std::vector<Instruction> instructions;

void GetRelevantData()
{
	// Read dummy file
	std::string path = "../data/dummy.bin";
	std::ifstream file{path, std::ios::binary | std::ios::ate};
	if (file.fail())
	{
		throw std::runtime_error("File not found");
	}

	std::size_t fileSize = file.tellg();
	file.seekg(0);

	std::vector<byte> * binDump = new std::vector<byte>(); 
	binDump->resize(fileSize); // Reserve a buffer the size of the file
	file.read(reinterpret_cast<char *>(binDump->data()), fileSize);

	std::map<std::string, std::vector<byte>> * codeSegment = new std::map<std::string, std::vector<byte>>();
	// Arbirtary name ".init" because it is one of the accepted section identifiers
	codeSegment->insert({std::string(".init"), *binDump});
	std::unique_ptr<Arch_x86> arch = std::make_unique<Arch_x86>(codeSegment);

	assembly = arch->TranslateToAssembly();
	instructions = arch->GetInstructionData();

	delete binDump;
}

TEST_CASE("x86 machine code section disassembly works properly", "[x86]")
{
	GetRelevantData();	

	REQUIRE (assembly.size() > 0);

	REQUIRE (assembly.size() == knownCorrectAssembly.size());

	std::vector<std::string> asmTemp = assembly;
	std::vector<std::string> correctAsmTemp = knownCorrectAssembly;
	for (auto line : asmTemp)
	{
		// Convert to lowercase
		std::remove_if(line.begin(), line.end(), isspace);
	}
	
	REQUIRE (assembly == knownCorrectAssembly);

	// Operand addressing methods that only take a register and not a numeric
	const std::vector<AddrMethod> regOnly
	{
		AddrMethod::B, 
		AddrMethod::C, 
		AddrMethod::D, 
		AddrMethod::F, 
		AddrMethod::G, 
		AddrMethod::H, 
		AddrMethod::L, 
		AddrMethod::N, 
		AddrMethod::P, 
		AddrMethod::R, 
		AddrMethod::S, 
		AddrMethod::U, 
		AddrMethod::V, 
		AddrMethod::X, 
		AddrMethod::Y, 
		AddrMethod::HARDCODED_REGISTER
	};
	
	// Operand addressing methods that only take a numeric and not a register
	const std::vector<AddrMethod> numOnly
	{

		AddrMethod::A, 
		AddrMethod::E, 
		AddrMethod::I, 
		AddrMethod::J, 
		AddrMethod::M, 
		AddrMethod::O, 
		AddrMethod::Q, 
		AddrMethod::W
	};
	
	for (auto instr : instructions)
	{
		// Operand 1	
		REQUIRE 
		(
			instr.op1.isRegister !=
			(std::find(regOnly.begin(), regOnly.end(), instr.op1.addrMethod) != regOnly.end())
		);
		
		// Operand 2	
		REQUIRE 
		(
			instr.op2.isRegister !=
			(std::find(regOnly.begin(), regOnly.end(), instr.op2.addrMethod) != regOnly.end())
		);
	}
}
