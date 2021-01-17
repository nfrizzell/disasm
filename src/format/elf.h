#pragma once

#include <vector>
#include <memory>
#include <array>
#include <elf.h>

#include "format.h"

#include "../util/common.h"
#include "../util/util.h"

class FormatELF : public Format
{
public:
	static std::unique_ptr<FormatELF> NewFormatELF(const std::vector<byte> * binDump);
	static bool IsFormat(const std::vector<byte> * binDump);
	void LoadMetadata(Metadata &metadata);
	Segment GetCodeSegment();
protected:
	// Using the 64-bit version of ELF structs because they work for both architectures
	// and allow for code reuse through inheritance, keep this in mind before using
	// sizeof(), decltype, etc. 
	Elf64_Ehdr elfHeader;
	std::vector<Elf64_Phdr> programHeaders;
	std::vector<Elf64_Shdr> sectionHeaders;

	Elf64_Shdr secStrTabHeader;
	std::vector<byte> secStrTabData;

	static std::array<byte, EI_NIDENT> LoadIdent(const std::vector<byte> * binDump);

	std::string LoadStringTableEntry(unsigned long strTabOff, unsigned long entryOff);

	void ParseBinDump();
	virtual void LoadELFHeader() = 0;
	virtual void LoadProgramHeaders() = 0;
	virtual void LoadSectionHeaders() = 0;
};

class FormatELF32 final : public FormatELF
{
public:
	FormatELF32(const std::vector<byte> * binDump);
	
private:  
	void LoadELFHeader();
	void LoadProgramHeaders();
	void LoadSectionHeaders();
};

class FormatELF64 final : public FormatELF
{
public:
	FormatELF64(const std::vector<byte> * binDump);
	
private:
	void LoadELFHeader();
	void LoadProgramHeaders();
	void LoadSectionHeaders();
};
