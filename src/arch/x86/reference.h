#pragma once

#include <vector>
#include <map>
#include <unordered_map>

#include "../../util/common.h"

namespace ISet_x86
{

const std::vector<byte> prefixes
{
        // Group 1
        0xF0,		// Lock
        0xF2,		// REPNE/REPNZ
        0xF3,		// Rep/REPE/REPZ

        // Group 2	// Segment overrides
        0x2E,		// CS / branch not taken
        0x36,		// SS
        0x3E,		// DS / branch taken:w

        0x26,		// ES
        0x64,		// FS
        0x65,		// GS

        // Group 3
        0x66,		// Operand-size modifier

        // Group 4
        0x67,		// Address-size modifier

        /*
        // REX (64 bit)
        0x40,
        0x41,
        0x42,
        0x43,
        0x44,
        0x45,
        0x46,
        0x47,
        0x48,
        0x49,
        0x4A,
        0x4B,
        0x4C,
        0x4D,
        0x4E,
        0x4F
        */
};

// The range that register types fall into in the enum
const int REGISTER_LOWER_BOUND = 100;
const int REGISTER_UPPER_BOUND = 2000;

enum class AddrMethod : int16_t
// These are the operand addressing methods as outlined in section A.2.1 of the
// Intel x86 software developer's manual
// Commented-out values are those that are in the documentation but not used here
{
        NOT_APPLICABLE = INVALID,
        A = 0, // Direct address, address of operand is encoded in instruction
        B = 1, // General-purpose register encoded in the vvvv field of the VEX prefix
        C = 2, // Control register encoded in the reg field of the ModR/M byte
        D = 3, // Debug register encoded in the reg field of the ModR/M byte

        E = 4, // ModR/M follows opcode and specifies operand, which can be either
               // a general-purpose register or a memory address. If it is a memory
               // address, the address is computed from a segment register and:
               // 	- a base register
               // 	- an index register
               // 	- a scaling factor
               // 	- a displacement

        F = 5, // The operand is located in the EFLAGS/RFLAGS register
        G = 6, // Reg field of the ModR/M byte encodes a general-purpose register
        H = 7,
        I = 8, // Immediate data
        J = 9, // The instruction encodes a relative offset to add to the instruction
               // pointer register
        L =10, // The upper 4 bits of the 8 bit immediate encode an XMM or YMM register
        M =11, // The ModR/M byte may refer only to memory
        N =12, // The R/M field of the ModR/M byte encodes a packed-quadword MMX register
        O =13, // No ModR/M byte, the offset of the operand is encoded as a word/dword in
                // the instruction
        P =14, // The reg field of the ModR/M byte encodes a packed quadword MMX register

        Q =15, // The ModR/M byte specifies the operand, which is either an MMX register
                // or a memory address. If it is a memory address, the address is computed
               // from a segment register and:
               //       - a base register
               //       - an index register
               //       - a scaling factor
               //       - a displacement

        R =16, // The R/M field of the ModR/M byte encodes a general-purpise register
        S =17, // The reg field of the ModR/M byte encodes a segment register

        U =18, // The R/M field of the ModR/M byte encodes a XMM register or YMM register
        V =19, // The reg field of the ModR/M byte encodes a XMM register or YMM register

        W =20, // The ModR/M byte specifies the operand, which is either a XMM register,
               // YMM register, or a memory address. If it is a memory address, the
               // address is computed from a segment register and:
               //       - a base register
               //       - an index register
               //       - a scaling factor
               //       - a displacement

        X =21, // Memory addressed by the DS and rSI register pair
        Y =22, // Memory addressed by the ES and rDI register pair

        // Extended operand addressing methods
        Z =23,
        BA=24,
        BB=25,
        BD=26,
        ES_=27,
        EST=28,
        SC=29,
        T=30,

        HARDCODED_VALUE = 100,

        // General-purpose registers
        AH = 101, AL = 102, AX = 103, EAX = 104,
        BH = 201, BL = 202, BX = 203, EBX = 204,
        CH = 301, CL = 302, CX = 303, ECX = 304,
        DH = 401, DL = 402, DX = 403, EDX = 404,
        // Pointer registers
        SI = 501, ESI = 502,
        DI = 601, EDI = 602,
        BP = 701, EBP = 702,
        SP = 801, ESP = 802,
        // Segment registers
        SS = 901, CS = 902, DS = 903, ES = 904, FS = 905, GS = 906,
        // Instruction pointer
        IP = 1001, EIP = 1002,
        // EFLAGS register
        EFLAGS = 1101,

        // FPU/x87 stack
        X87_STACK_TOP = 1201,
        X87_STACK_1 = 1202,
        X87_STACK_2 = 1203,
        X87_STACK_3 = 1204,
        X87_STACK_4 = 1205,
        X87_STACK_5 = 1206,
        X87_STACK_6 = 1207,
        X87_STACK_7 = 1208
};


enum class OperandType : int16_t
{
        NOT_APPLICABLE = INVALID,
        a = 0, // Two one-word operands or double-word operands in memory, depending on
               // operand-size attribute
        b = 1, // Byte
        c = 2, // Byte or word, depending on operand-size attribute
        d = 3, // Doubleword
        dq= 4, // Double-quadword
        p = 5, // 32-bit, 48-bit, or 80-bit pointer, depending on operand-size attribute
        pd= 6, // 128-bit or 256-bit packed double-precision floating-point data
        pi= 7, // Quadword MMX register
        ps= 8, // 128-bit or 256-bit packed single-precision floating-point data
        q = 9, // Quadword
        qq=10, // Quad-quadword
        s =11, // 6-byte psuedo-descriptor
        sd=12, // Scalar element of a 128-bit double-precision floating data
        ss=13, // Scalar element of a 128-bit single-precision floating data
        si=14, // Doubleword integer register (e.g. EAX)
        v =15, // Word, doubleword, or quadword, depending on operand-size attribute
        w =16, // Word
        x =17, // Double-quadword or quad-quadword, depending on operand-size attribute
        y =18, // Doubleword or quadword, depending on operand-size attribute
        z =19, // Word for 16-bit operand-size or doubleword for 32/64-bit operand-size

        // Extended operand types
        /*
        Some of these may not be intuitively in order. Don't pay too much attention to the numbers themselves,
        they are just magic numbers that are used to identify each possible attribute and have no actual
        meaning beyond this. If you want to know what each attribute represents, the names match those
        used in relevant documentation.
        */

        bcd=20,
        bs=21,
        bsq=22,
        bss=23,
        di=24,
        dqp=25,
        dr=26,
        e=27,
        er=28,
        psq=29,
        ptp=30,
        qi=31,
        sr=32,
        st=33,
        stx=34,
        vds=35,
        vqp=36,
        vs=37,
        wi=38,
        va=39,
        dqa=40,
        wa=41,
        wo=42,
        ws=43,
        da=44,
        do_=45,
        qa=46,
        qs=47,
        vq=48,

        // General-purpose registers
        AH = 101, AL = 102, AX = 103, EAX = 104,
        BH = 201, BL = 202, BX = 203, EBX = 204,
        CH = 301, CL = 302, CX = 303, ECX = 304,
        DH = 401, DL = 402, DX = 403, EDX = 404,
        // Pointer registers
        SI = 501, ESI = 502,
        DI = 601, EDI = 602,
        BP = 701, EBP = 702,
        SP = 801, ESP = 802,
        // Segment registers
        SS = 901, CS = 902, DS = 903, ES = 904, FS = 905, GS = 906,
        // Instruction pointer
        IP = 1001, EIP = 1002,
        // EFLAGS register
        EFLAGS = 1101,

        // FPU/x87 stack
        X87_STACK_TOP = 1201,
        X87_STACK_1 = 1202,
        X87_STACK_2 = 1203,
        X87_STACK_3 = 1204,
        X87_STACK_4 = 1205,
        X87_STACK_5 = 1206,
        X87_STACK_6 = 1207,
        X87_STACK_7 = 1208,

        // Other special types
        IMMD = 2001, // Immediate value
        HARDCODED_VALUE = 2101, //  For instructions that have intrinsic constant operands
        UNSUPPORTED = 2201 // Complex instructions that don't fit the usual form
};

class Opcode;
class OpcodeHash;
class Instruction;

class InstructionReference
{
public:
    bool Contains(Opcode opkey);
    bool ContainsPrimary(byte b);
    Instruction GetReference(Opcode opkey);
    void Emplace(Opcode opkey, Instruction instr);
    int size();
    int count(Opcode key);
};

extern const std::map<int, AddrMethod> ModRMRegisterEncoding8;
extern const std::map<int, AddrMethod> ModRMRegisterEncoding16;
extern const std::map<int, AddrMethod> ModRMRegisterEncoding32;
extern const std::map<int, AddrMethod> SIBIndex;
extern const std::map<int, AddrMethod> SIBBase;
extern InstructionReference instrReference;

};
