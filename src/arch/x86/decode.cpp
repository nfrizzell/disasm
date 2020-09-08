#include <utility>
#include <iostream>
#include <algorithm>

#include "decode.h"

namespace ISet_x86
{

using namespace State_x86;

// ***** LinearDecoder *****
LinearDecoder::LinearDecoder(std::vector<byte> * section)
{
	this->section = section;

	instructions = std::vector<Instruction>();
	currentInstr = Instruction();
	currentByte = section->at(byteOffset);
	state = Init;
}

std::vector<Instruction> LinearDecoder::DecodeSection()
{
	// State machine rules
	// ===================
	// 1. Only call NextByte whenever the next byte is needed, and never to prepare for the
	// next state. This is for maximum clarity, and prevents prematurely reaching EndOfSegment
	// 2. Each call to NextByte should have its return value checked, and change state to
	// EndOfSegment if necessary
	// 3. The general flow of state transfer should look something like {Init -> ... ->
	// DecodeSuccess/DecodeError -> Init} until EndOfSegment is reached
	while (state != EndOfSegment)
	{
		state(this, currentInstr);
	}	

	return instructions;
}

byte LinearDecoder::CurrentByte()
{
	return currentByte;
}

unsigned int LinearDecoder::ByteOffset()
{
	return byteOffset;
}

bool __attribute__((warn_unused_result)) LinearDecoder::NextByte()
{
	if (byteOffset + 1 < section->size())
	{
		byteOffset++;
		currentByte = section->at(byteOffset);
		return true;
	}

	else
	{
		return false;
	}
}

void LinearDecoder::NextInstruction()
{
	instructions.push_back(currentInstr);
	currentInstr = Instruction();
}
	
void LinearDecoder::ChangeState(funcptr newState)
{
	if (processFlags.debug)
	{
		const int loopMax = 100; // Abritrary limit on same-state loops
		if (&state == &newState)
		{
			stateLoopCounter++;
			std::cerr << "state: " << &state << '\n';
			std::cerr << "new state: " << &newState << '\n';
		}

		// Used to make sure the program doesn't enter an infinite loop when parsing
		// this will probably be removed later but for now it is a nice debugging feature
		if (stateLoopCounter > loopMax)
		{
			std::string error = "Process entered infinite loop while parsing x86 instructions. Details:'\n'currentByte = "; 
			error += currentByte;  ;
			error += "'\n'byteOffset = ";
			error += byteOffset;
			error += '\n';
			throw std::runtime_error(error);
		}	
	}

	state = newState;		
}

bool InstructionReference::Contains(Opcode opcode)
{
	return reference.find(opcode) != reference.end();
}

bool InstructionReference::ContainsPrimary(byte b)
{
	for (auto instr : reference)
	{
		if (instr.second.encoded.opcode.primary == b)
		{
			return true;
		}
	}
			
	return false;
}

Instruction InstructionReference::GetReference(Opcode opcode)
{
	return reference.at(opcode);
}

void InstructionReference::Emplace(Opcode opcode, Instruction instruction)
{
	reference.emplace(opcode, instruction);
}

InstructionReference instrReference {};

};

namespace State_x86
{

// General states
void Init(LinearDecoder * context, Instruction &instr)
{
	instr.attrib.runtime.segmentByteOffset = context->ByteOffset();
	context->ChangeState(Prefix);
}

void Prefix(LinearDecoder * context, Instruction &instr)
{
	// Advance the byte pointer if the current one has already been parsed as a prefix	
	if (instr.attrib.runtime.prefixCount > 0)
	{
		if(!context->NextByte())
		{
			context->ChangeState(EndOfSegment);
			return;
		}
	}

	// Current byte recognized as a valid prefix 
	if (std::find(prefixes.begin(), prefixes.end(), context->CurrentByte()) != prefixes.end())
	{
		instr.encoded.prefix[instr.attrib.runtime.prefixCount] = context->CurrentByte();
		instr.attrib.runtime.prefixCount++;
	}
	
	// Current byte not recognized as a prefix, will be checked for opcode
	else
	{
		context->ChangeState(Opcode);
	}
}

// Assumes the two-byte flag (0x0F) is present, since all three-byte opcodes will have it
// TBD
const std::vector<int> threeByteOpcodes
{
};

void Opcode(LinearDecoder * context, Instruction &instr)
{ 
	// Advance the byte pointer if a prefix was decoded or two-byte opcode indicated
	if (instr.attrib.runtime.prefixCount > 0 || instr.encoded.opcode.twoByte)
	{
		if(!context->NextByte())
		{
			context->ChangeState(EndOfSegment);
			return;
		}
	}

	// Current byte signals a two-byte opcode	
	if (context->CurrentByte() == 0x0F) 
	{
		instr.encoded.opcode.twoByte = true;
		instr.attrib.runtime.opcodeLength++;
	}

	// Current byte is a valid primary opcode (including those that follow the two-byte signal)
	else if (instrReference.ContainsPrimary(context->CurrentByte()))
	{
		instr.encoded.opcode.primary = context->CurrentByte();
		instr.attrib.runtime.opcodeLength++;

		// Check for three-byte opcode
		if (instr.encoded.opcode.twoByte && std::find(threeByteOpcodes.begin(), threeByteOpcodes.end(), context->CurrentByte()) != threeByteOpcodes.end())
		{
			if(!context->NextByte())
			{
				context->ChangeState(EndOfSegment);
				return;
			}

			instr.encoded.opcode.secondary = context->CurrentByte();
			instr.attrib.runtime.opcodeLength++;
		}

		// Update the instruction based upon the common attributes inferred from opcode
		const Instruction reference = instrReference.GetReference(instr.encoded.opcode);
		instr.UpdateAttributes(reference);

		context->ChangeState(Operands);
	}

	// No opcode byte was found in the byte sequence, which means decoding cannot continue
	else
	{
		context->ChangeState(DecodeFailure);
	}	
}

void Operands(LinearDecoder * context, Instruction &instr)
{
	// If the instruction has a first operand that hasn't been read yet
	if (instr.op1.attrib.intrinsic.type != OperandType::NOT_APPLICABLE && !instr.attrib.runtime.op1Read)
	{
		instr.activeOperand = &instr.op1;
		context->ChangeState(addrMethodHandler.at(instr.op1.attrib.intrinsic.addrMethod));
		instr.attrib.runtime.op1Read = true;
	}

	// If the instruction has a second operand and the first has already been read
	else if (instr.op2.attrib.intrinsic.type != OperandType::NOT_APPLICABLE && instr.attrib.runtime.op1Read && !instr.attrib.runtime.op2Read)
	{
		instr.activeOperand = &instr.op2;
		context->ChangeState(addrMethodHandler.at(instr.op2.attrib.intrinsic.addrMethod));
		instr.attrib.runtime.op2Read = true;
	}

	// Either there are no operands and decoding is finished, or the operand(s) have been
	// decoded and decoding of instruction is finished
	else 
	{
		context->ChangeState(DecodeSuccess);
	}
}

void DecodeSuccess(LinearDecoder * context, Instruction &instr)
{
	// Prepare for next instruction
	if(!context->NextByte())
	{
		context->ChangeState(EndOfSegment);
		return;
	}

	instr.attrib.runtime.resolved = true;
	instr.attrib.runtime.size = (context->ByteOffset() - instr.attrib.runtime.segmentByteOffset) + 1;
	context->NextInstruction(); // Handle parsed instruction and prepare for new one

	context->ChangeState(Init); // Reset state
}

void DecodeFailure(LinearDecoder * context, Instruction &instr)
{
	// Nothing useful could be done with the current byte, so advance the byte pointer
	if(!context->NextByte())
	{
		context->ChangeState(EndOfSegment);
		return;
	}

	instr.attrib.runtime.resolved = false;
	instr.attrib.runtime.size = (context->ByteOffset() - instr.attrib.runtime.segmentByteOffset) + 1;
	context->NextInstruction(); // Handle parsed instruction and prepare for new one

	context->ChangeState(Init); // Reset state
}

void EndOfSegment(LinearDecoder * context, Instruction &instr)
{
	instr.attrib.runtime.size = (context->ByteOffset() - instr.attrib.runtime.segmentByteOffset) + 1;
	// Either dump the current "instruction" as a data sequence or handle it as a complete
	// instruction, depending on whether or not it was finished parsing before the EOS state
	context->NextInstruction();
}

// Addressing mode states
void MethodError(LinearDecoder * context, Instruction &instr)
{
	throw std::runtime_error("MethodError state reached in x86 state machine.");
}

void MethodA(LinearDecoder * context, Instruction &instr)
{
	context->ChangeState(Operands);
}

void MethodB(LinearDecoder * context, Instruction &instr)
{
	context->ChangeState(Operands);

}

void MethodC(LinearDecoder * context, Instruction &instr)
{
	context->ChangeState(Operands);

}

void MethodD(LinearDecoder * context, Instruction &instr)
{
	context->ChangeState(Operands);

}

void MethodE(LinearDecoder * context, Instruction &instr)
{
	if (!context->NextByte())
	{
		context->ChangeState(EndOfSegment); 
		return;
	}

	byte modrmByte = context->CurrentByte();
	instr.InterpretModRMByte(modrmByte);
	
	if (instr.attrib.runtime.hasSIB)
	{
		// Get SIB byte
		if (!context->NextByte())
		{
			context->ChangeState(EndOfSegment); 
			return;
		}
		
		byte sibByte = context->CurrentByte();
		instr.InterpretSIBByte(sibByte);
	}
	
	context->ChangeState(Operands);
}

void MethodF(LinearDecoder * context, Instruction &instr)
{
	context->ChangeState(Operands);

}

void MethodG(LinearDecoder * context, Instruction &instr)
{
	context->ChangeState(Operands);
}
 
void MethodH(LinearDecoder * context, Instruction &instr)
{
	context->ChangeState(Operands);

}

void MethodI(LinearDecoder * context, Instruction &instr)
{
	for (int i = 0; i < instr.activeOperand->attrib.intrinsic.size; i++)
	{
		if (!context->NextByte())
		{
			context->ChangeState(EndOfSegment); 
			return;
		}

		instr.encoded.immd[i] = context->CurrentByte();
	}
	
	instr.activeOperand->value = OperandType::IMMD;
	context->ChangeState(Operands);
}

void MethodJ(LinearDecoder * context, Instruction &instr)
{
	context->ChangeState(Operands);

}

void MethodL(LinearDecoder * context, Instruction &instr)
{
	context->ChangeState(Operands);

}

void MethodM(LinearDecoder * context, Instruction &instr)
{
	context->ChangeState(Operands);

}

void MethodN(LinearDecoder * context, Instruction &instr)
{
	context->ChangeState(Operands);

}

void MethodO(LinearDecoder * context, Instruction &instr)
{
	context->ChangeState(Operands);

}

void MethodP(LinearDecoder * context, Instruction &instr)
{
	context->ChangeState(Operands);

}

void MethodQ(LinearDecoder * context, Instruction &instr)
{
	context->ChangeState(Operands);

}

void MethodR(LinearDecoder * context, Instruction &instr)
{
	context->ChangeState(Operands);

}

void MethodS(LinearDecoder * context, Instruction &instr)
{
	context->ChangeState(Operands);

}

void MethodU(LinearDecoder * context, Instruction &instr)
{
	context->ChangeState(Operands);

}

void MethodV(LinearDecoder * context, Instruction &instr)
{
	context->ChangeState(Operands);

}

void MethodW(LinearDecoder * context, Instruction &instr)
{
	context->ChangeState(Operands);

}

void MethodX(LinearDecoder * context, Instruction &instr)
{
	context->ChangeState(Operands);

}

void MethodY(LinearDecoder * context, Instruction &instr)
{
	context->ChangeState(Operands);

}

void MethodRegister(LinearDecoder * context, Instruction &instr)
{
	OperandType reg = instr.activeOperand->attrib.intrinsic.type;
	instr.activeOperand->value = reg;
	context->ChangeState(Operands);
}

};
