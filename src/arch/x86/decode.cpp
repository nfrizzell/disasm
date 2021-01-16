#include <utility>
#include <iostream>
#include <sstream>
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
	currentInstr.encoded.encodedSequence.push_back(currentByte);

	if (byteOffset + 1 < section->size())
	{
		byteOffset++;
		currentByte = section->at(byteOffset);
		return true;
	}

	return false;
}

bool LinearDecoder::ReverseByte()
{
	if (byteOffset > 0)
	{
		byteOffset--;
		currentByte = section->at(byteOffset);
		return true;
	}

	return false;
}

void LinearDecoder::NextInstruction()
{
	instructions.push_back(currentInstr);
	currentInstr = Instruction();
}
	
void LinearDecoder::ChangeState(funcptr newState)
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

	state = newState;		
}

funcptr AddrMethodHandler::at(AddrMethod method)
{
	if (addrMethodHandlers.count(method) > 0)
	{
		return addrMethodHandlers.at(method);
	}

	// If the addressing method is a register
	else if (static_cast<int>(method) > REGISTER_LOWER_BOUND && static_cast<int>(method) < REGISTER_UPPER_BOUND)
	{
		return MethodRegister;
	}

	else
	{
		return MethodError;
	}
}

AddrMethodHandler addrMethodHandler {};

std::unordered_map<ThreeByteKey, uint16_t, ThreeByteHash> threeByteReference {};

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

void Opcode(LinearDecoder * context, Instruction &instr)
{ 
	// Current byte signals a two-byte opcode	
	if (context->CurrentByte() == 0x0F) 
	{
		instr.encoded.opcode.twoByte = true;
		instr.attrib.runtime.opcodeLength++;

		if(!context->NextByte())
		{
			context->ChangeState(EndOfSegment);
			return;
		}
	}

	// Current byte is a valid primary opcode (including those that follow the two-byte signal)
	if (instrReference.ContainsPrimary(context->CurrentByte()))
	{
		instr.encoded.opcode.primary = context->CurrentByte();
		instr.attrib.runtime.opcodeLength++;

		ThreeByteKey tbk = { instr.encoded.opcode.twoByte, instr.encoded.opcode.primary };

		// Check whether or not a secondary opcode will be present
		if (threeByteReference.find(tbk) != threeByteReference.end())
		{
			if(!context->NextByte())
			{
				context->ChangeState(EndOfSegment);
				return;
			}

			auto secondary = context->CurrentByte();

			if (threeByteReference.at(tbk) == secondary)
			{
				instr.encoded.opcode.secondary = secondary;
				instr.attrib.runtime.opcodeLength++;
			}

			else
			{
				context->ReverseByte();
			}
		}

		// Update the instruction based upon the common attributes inferred from opcode
		const Instruction reference = instrReference.GetReference(instr.encoded.opcode);
		if (reference.attrib.intrinsic.mnemonic == "UNRESOLVED")
		{
			context->ChangeState(DecodeFailure);
		}
		else
		{
			instr.UpdateAttributes(reference);

			context->ChangeState(Operands);
		}
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
    if (instr.op1.attrib.intrinsic.type != OperandType::NOT_APPLICABLE && !instr.attrib.flags.op1Read)
	{
        context->ChangeState(addrMethodHandler.at(instr.op1.attrib.intrinsic.addrMethod));
		instr.attrib.flags.op1Read = true;
	}

	// If the instruction has a second operand and the first has already been read
    else if (instr.op2.attrib.intrinsic.type != OperandType::NOT_APPLICABLE && instr.attrib.flags.op1Read && !instr.attrib.flags.op2Read)
	{
        context->ChangeState(addrMethodHandler.at(instr.op2.attrib.intrinsic.addrMethod));
		instr.attrib.flags.op2Read = true;
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

	instr.attrib.flags.resolved = true;
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

void MethodUnimplemented(LinearDecoder * context, Instruction &instr)
{
	context->ChangeState(Operands);
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
	if (!instr.attrib.flags.modRMRead)
	{

		// Get the ModRM byte
		if (!context->NextByte())
		{
			context->ChangeState(EndOfSegment); 
			return;
		}

		byte modrmByte = context->CurrentByte();
		instr.InterpretModRMByte(modrmByte);
	}

	if (instr.attrib.flags.hasSIB && !instr.attrib.flags.sibRead)
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

	if (instr.attrib.flags.hasDisplacement && !instr.attrib.flags.dispRead)
	{
		for (int i = 0; i < instr.attrib.runtime.displacementSize; i++)
		{
			if (!context->NextByte())
			{
				context->ChangeState(EndOfSegment); 
				return;
			}

			instr.encoded.disp += context->CurrentByte();
		}

		instr.attrib.flags.dispRead = true;
	}

	context->ChangeState(Operands);
}

void MethodF(LinearDecoder * context, Instruction &instr)
{
	context->ChangeState(Operands);

}

void MethodG(LinearDecoder * context, Instruction &instr)
{
	if (!instr.attrib.flags.modRMRead)
	{

		// Get the ModRM byte
		if (!context->NextByte())
		{
			context->ChangeState(EndOfSegment); 
			return;
		}

		byte modrmByte = context->CurrentByte();
		instr.InterpretModRMByte(modrmByte);
	}

	if (instr.attrib.flags.hasSIB && !instr.attrib.flags.sibRead)
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

	if (instr.attrib.flags.hasDisplacement && !instr.attrib.flags.dispRead)
	{
		for (int i = 0; i < instr.attrib.runtime.displacementSize; i++)
		{
			if (!context->NextByte())
			{
				context->ChangeState(EndOfSegment); 
				return;
			}

			instr.encoded.disp += context->CurrentByte();
		}
		instr.attrib.flags.dispRead = true;
	}

	context->ChangeState(Operands);
}
 
void MethodH(LinearDecoder * context, Instruction &instr)
{
	context->ChangeState(Operands);

}

// IMMD data
void MethodI(LinearDecoder * context, Instruction &instr)
{
    bool sField = instr.encoded.opcode.primary & 0b00000001;
    bool xField = instr.encoded.opcode.primary & 0b00000010;
	int immdSize = 0;

	if (sField)
	{
		immdSize = 4;
	}
	else
	{
		immdSize = 1;
	}
	// Overrides
	if (xField)
	{
		immdSize = 1;	
	}

	for (int i = 0; i < immdSize; i++)
	{
		if (!context->NextByte())
		{
			context->ChangeState(EndOfSegment); 
			return;
		}

		instr.encoded.immd += (context->CurrentByte() << (8*i));
	}
	
	context->ChangeState(Operands);
}

// This one is messy, I need to come back and see if it can be fixed later
void MethodJ(LinearDecoder * context, Instruction &instr)
{
	// Behaves differently than the others
	if (instr.attrib.intrinsic.mnemonic == "CALL")
	{
		for (int i = 0; i < 4; i++)
		{
			if (!context->NextByte())
			{
				context->ChangeState(EndOfSegment);
				return;
			}

			instr.attrib.runtime.displacementSize = 4;
			instr.encoded.disp += context->CurrentByte();
		}
	}

	else
	{
		if (!context->NextByte())
		{
			context->ChangeState(EndOfSegment);
			return;
		}
		instr.attrib.runtime.displacementSize = 1;
		instr.encoded.disp = context->CurrentByte();
	}
	
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

void MethodZ(LinearDecoder * context, Instruction &instr)
{
	uint8_t relevantOpcodeSection = instr.encoded.opcode.primary & 0b00000111;
	AddrMethod encodedReg = ModRMRegisterEncoding32.at(relevantOpcodeSection);

	context->ChangeState(Operands);
}

void MethodRegister(LinearDecoder * context, Instruction &instr)
{
	context->ChangeState(Operands);
}

};
