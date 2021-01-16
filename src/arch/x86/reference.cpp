#include "reference.h"
#include "instruction.h"

namespace ISet_x86
{

// If a type is not listed here, it is because it is either hard to find information on or is
// simply not yet implemented
std::map<OperandType, uint8_t> typeSize16
{
    {OperandType::NOT_APPLICABLE, 0},
    {OperandType::a, 4},
    {OperandType::b, 1},
    {OperandType::c, 1},
    {OperandType::d, 4},
    {OperandType::dq, 16},
    {OperandType::p, 4},
    {OperandType::pd, 16},
    {OperandType::pi, 8},
    {OperandType::ps, 16},
    {OperandType::q, 8},
    {OperandType::qq, 32},
    {OperandType::s, 6},
    {OperandType::sd, 16},
    {OperandType::ss, 16},
    {OperandType::si, 4},
    {OperandType::v, 2},
    {OperandType::w, 2},
    {OperandType::x, 8},
    {OperandType::y, 4},
    {OperandType::z, 2},

    // These are other types that I haven't gotten around to figuring out what size
    // data they represent yet
    {OperandType::bcd, 2},
    {OperandType::bs, 2},
    {OperandType::bsq, 2},
    {OperandType::bss, 2},
    {OperandType::di, 2},
    {OperandType::dqp, 2},
    {OperandType::dr, 2},
    {OperandType::e, 2},
    {OperandType::er, 2},
    {OperandType::psq, 2},
    {OperandType::ptp, 2},
    {OperandType::qi, 2},
    {OperandType::sr, 2},
    {OperandType::st, 2},
    {OperandType::stx, 2},
    {OperandType::vds, 2},
    {OperandType::vqp, 2},
    {OperandType::vs, 2},
    {OperandType::wi, 2},
    {OperandType::va, 2},
    {OperandType::dqa, 2},
    {OperandType::wa, 2},
    {OperandType::wo, 2},
    {OperandType::ws, 2},
    {OperandType::da, 2},
    {OperandType::do_, 2},
    {OperandType::qa, 2},
    {OperandType::qs, 2},
    {OperandType::vq, 2},

    {OperandType::AH, 2},
    {OperandType::AL, 2},
    {OperandType::AX, 2},
    {OperandType::EAX, 4},
    {OperandType::BH, 2},
    {OperandType::BL, 2},
    {OperandType::BX, 2},
    {OperandType::EBX, 4},
    {OperandType::CH, 2},
    {OperandType::CL, 2},
    {OperandType::CX, 2},
    {OperandType::ECX, 4},
    {OperandType::DH, 2},
    {OperandType::DL, 2},
    {OperandType::DX, 2},
    {OperandType::EDX, 4},
    {OperandType::SI, 2},
    {OperandType::ESI, 4},
    {OperandType::DI, 2},
    {OperandType::EDI, 4},
    {OperandType::BP, 2},
    {OperandType::EBP, 4},
    {OperandType::SP, 2},
    {OperandType::ESP, 4},
    {OperandType::SS, 2},
    {OperandType::CS, 2},
    {OperandType::DS, 2},
    {OperandType::ES, 2},
    {OperandType::FS, 2},
    {OperandType::GS, 2},
    {OperandType::IP, 2},
    {OperandType::EIP, 4},
    {OperandType::EFLAGS, 2}
};

// If a type is not listed here, it is because it is either hard to find information on or is
// simply not yet implemented
std::map<OperandType, uint8_t> typeSize32
{
    {OperandType::NOT_APPLICABLE, 0},
    {OperandType::a, 8},
    {OperandType::b, 1},
    {OperandType::c, 2},
    {OperandType::d, 4},
    {OperandType::dq, 16},
    {OperandType::p, 6},
    {OperandType::pi, 8},
    {OperandType::ps, 16},
    {OperandType::q, 8},
    {OperandType::s, 6},
    {OperandType::si, 4},
    {OperandType::ss, 16},
    {OperandType::v, 4},
    {OperandType::w, 2},
    {OperandType::x, 16},
    {OperandType::y, 8},
    {OperandType::z, 8},

    // These are other types that I haven't gotten around to figuring out what size
    // data they represent yet
    {OperandType::bcd, 2},
    {OperandType::bs, 2},
    {OperandType::bsq, 2},
    {OperandType::bss, 2},
    {OperandType::di, 2},
    {OperandType::dqp, 2},
    {OperandType::dr, 2},
    {OperandType::e, 2},
    {OperandType::er, 2},
    {OperandType::psq, 2},
    {OperandType::ptp, 2},
    {OperandType::qi, 2},
    {OperandType::sr, 2},
    {OperandType::st, 2},
    {OperandType::stx, 2},
    {OperandType::vds, 2},
    {OperandType::vqp, 2},
    {OperandType::vs, 2},
    {OperandType::wi, 2},
    {OperandType::va, 2},
    {OperandType::dqa, 2},
    {OperandType::wa, 2},
    {OperandType::wo, 2},
    {OperandType::ws, 2},
    {OperandType::da, 2},
    {OperandType::do_, 2},
    {OperandType::qa, 2},
    {OperandType::qs, 2},
    {OperandType::vq, 2},

    {OperandType::AH, 2},
    {OperandType::AL, 2},
    {OperandType::AX, 2},
    {OperandType::EAX, 4},
    {OperandType::BH, 2},
    {OperandType::BL, 2},
    {OperandType::BX, 2},
    {OperandType::EBX, 4},
    {OperandType::CH, 2},
    {OperandType::CL, 2},
    {OperandType::CX, 2},
    {OperandType::ECX, 4},
    {OperandType::DH, 2},
    {OperandType::DL, 2},
    {OperandType::DX, 2},
    {OperandType::EDX, 4},
    {OperandType::SI, 2},
    {OperandType::ESI, 4},
    {OperandType::DI, 2},
    {OperandType::EDI, 4},
    {OperandType::BP, 2},
    {OperandType::EBP, 4},
    {OperandType::SP, 2},
    {OperandType::ESP, 4},
    {OperandType::SS, 2},
    {OperandType::CS, 2},
    {OperandType::DS, 2},
    {OperandType::ES, 2},
    {OperandType::FS, 2},
    {OperandType::GS, 2},
    {OperandType::IP, 2},
    {OperandType::EIP, 4},
    {OperandType::EFLAGS, 2}
};

// Defined values of the REG field of the ModR/M byte if the data is 8 bits
const std::map<int, AddrMethod> ModRMRegisterEncoding8
{
    { 0b000, AddrMethod::AL },
    { 0b001, AddrMethod::CL },
    { 0b010, AddrMethod::DL },
    { 0b011, AddrMethod::BL },
    { 0b100, AddrMethod::AH },
    { 0b101, AddrMethod::CH },
    { 0b110, AddrMethod::DH },
    { 0b111, AddrMethod::BH }
};

// Defined values of the REG field of the ModR/M byte if the data is 16 bits
const std::map<int, AddrMethod> ModRMRegisterEncoding16
{
    { 0b000, AddrMethod::AX },
    { 0b001, AddrMethod::CX },
    { 0b010, AddrMethod::DX },
    { 0b011, AddrMethod::BX },
    { 0b100, AddrMethod::SP },
    { 0b101, AddrMethod::BP },
    { 0b110, AddrMethod::SI },
    { 0b111, AddrMethod::DI }
};

// Defined values of the REG field of the ModR/M byte if the data is 32 bits
const std::map<int, AddrMethod> ModRMRegisterEncoding32
{
    { 0b000, AddrMethod::EAX },
    { 0b001, AddrMethod::ECX },
    { 0b010, AddrMethod::EDX },
    { 0b011, AddrMethod::EBX },
    { 0b100, AddrMethod::ESP },
    { 0b101, AddrMethod::EBP },
    { 0b110, AddrMethod::ESI },
    { 0b111, AddrMethod::EDI }
};


std::unordered_map<Opcode, Instruction, OpcodeHash> instrReferenceMap;

// Aliases since these use the same mappings
const std::map<int, AddrMethod> SIBIndex = ModRMRegisterEncoding32;
const std::map<int, AddrMethod> SIBBase = ModRMRegisterEncoding32;

bool InstructionReference::Contains(Opcode opkey)
{
    return instrReferenceMap.find(opkey) != instrReferenceMap.end();
}

bool InstructionReference::ContainsPrimary(byte b)
{
    for (auto instr : instrReferenceMap)
    {
        if (instr.second.encoded.opcode.primary == b)
        {
            return true;
        }
    }

    return false;
}

Instruction InstructionReference::GetReference(Opcode opkey)
{
	if (instrReferenceMap.find(opkey) != instrReferenceMap.end())
	{
		return instrReferenceMap.at(opkey);
	}

	// Used for instructions which have an opcode extension, as otherwise
	// they would not be located at this stage properly
	auto copyWithOpcodeExtension = opkey;
	copyWithOpcodeExtension.extension = 0;

	if (instrReferenceMap.count(copyWithOpcodeExtension) > 0)
	{
		return instrReferenceMap.at(copyWithOpcodeExtension);
	}

    else
    {
        return Instruction {};
        /*
        std::stringstream err;
        err << std::hex << "Tried to lookup opcode that does not exist: " << opkey.mandatoryPrefix << " " << (int)opkey.twoByte << " " << opkey.primary << " " << opkey.secondary << " " << (int)opkey.extension;
        throw std::runtime_error(err.str());
        */
    }
}

int InstructionReference::size()
{
    return instrReferenceMap.size();
}

int InstructionReference::count(Opcode key)
{
    return instrReferenceMap.count(key);
}

void InstructionReference::Emplace(Opcode opkey, Instruction instruction)
{
    instrReferenceMap.emplace(opkey, instruction);
}
InstructionReference instrReference {};

}
