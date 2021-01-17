#include <stdexcept>
#include <algorithm>

#include "elf.h"

// ***** FormatELF *****

bool FormatELF::IsFormat(const std::vector<byte> * binDump)
{
	auto bd = *binDump;

	if(bd[0] == 0x7f
	&& bd[1] == 0x45
	&& bd[2] == 0x4c
	&& bd[3] == 0x46)
	{
		return true;
	}

	return false;
}

void FormatELF::LoadMetadata(Metadata &metadata)
{
	auto &eh = elfHeader;

	// Format
	metadata.format = FormatType::ELF;

	// Architecture
	if ((eh.e_machine == EM_386) && (eh.e_ident[EI_CLASS] == ELFCLASS32))
	{
		metadata.arch = ArchType::x86;
	}	

	else if ((eh.e_machine == EM_X86_64) && (eh.e_ident[EI_CLASS] == ELFCLASS64))
	{
		metadata.arch = ArchType::x64;
	}	

	else
	{
		metadata.arch = ArchType::UNDEFINED;
	}

	// Endianness
	if (eh.e_ident[EI_DATA] == ELFDATA2LSB)
	{
		metadata.end = EndType::LSB;
	}

	else if (eh.e_ident[EI_DATA] == ELFDATA2MSB)
	{
		metadata.end = EndType::MSB;
	}

	else
	{
		metadata.end = EndType::UNDEFINED; 
	}
};

std::unique_ptr<FormatELF> FormatELF::NewFormatELF(const std::vector<byte> * binDump)
{
	auto ident = FormatELF::LoadIdent(binDump);

	if (ident[EI_CLASS] == ELFCLASS32)
	{
		return std::make_unique<FormatELF32>(binDump);
	}

	else if (ident[EI_CLASS] == ELFCLASS64)
	{
		return std::make_unique<FormatELF64>(binDump);
	}

	throw std::runtime_error("File class unrecognized");
}

std::array<byte, EI_NIDENT> FormatELF::LoadIdent(const std::vector<byte> * binDump)
// Load identification values, which provide necessary information
// to load the rest of the header/file 
{
	auto ident = std::array<byte, EI_NIDENT>();
	for (int i = 0; i < EI_NIDENT; i++)  
	{
		ident[i] = binDump->at(i);
	}

	return ident;
}	
Segment FormatELF::GetCodeSegment()
{
	Segment segment {};
	const std::vector<std::string> id = {".plt", ".plt.got", ".init", ".text", ".fini"};
	segment.seg = new std::map<std::string, std::vector<byte>>;

	auto sstOff = sectionHeaders[elfHeader.e_shstrndx].sh_offset; // Section name string table
	for (auto sh : sectionHeaders)
	{
		std::string name = LoadStringTableEntry(sstOff, sh.sh_name);
		if (std::find(id.begin(), id.end(), name) != id.end())
		{
			auto begin = binDump->begin() + sh.sh_offset;
			auto end = begin + sh.sh_size;
			auto sectionCode = std::vector<byte>(begin, end);
			segment.seg->insert({name, sectionCode});
		}
	}

	return segment;
}

std::string FormatELF::LoadStringTableEntry(unsigned long strTabOff, unsigned long entryOff)
{
	auto begin = binDump->begin() + strTabOff + entryOff; // Beginning of entry/C string
	auto end = std::find(begin, binDump->end(), '\0'); // End of the C string (null term)

	std::string entry(begin, end);

	return entry;
}

void FormatELF::ParseBinDump() 
{
	LoadELFHeader();
	LoadProgramHeaders();
	LoadSectionHeaders();
}
 
// ***** FormatELF32 *****

FormatELF32::FormatELF32(const std::vector<byte> * binDump)
{
	this->binDump = binDump;
	ParseBinDump();
}

void FormatELF32::LoadELFHeader()
{
	ByteSequence bs(binDump, EI_NIDENT);
	// Using the 32-bit types for this struct because it is how they are actually represented in a 32-bit file
	auto &eh = elfHeader;
	auto ident = LoadIdent(binDump);
	for (int i = 0; i < EI_NIDENT; i++)  
	{
		eh.e_ident[i] = ident[i];
	}

	eh.e_type = bs.ReadBytes<Elf32_Half>();	
	eh.e_machine = bs.ReadBytes<Elf32_Half>();	
	eh.e_version = bs.ReadBytes<Elf32_Word>();	
	eh.e_entry = bs.ReadBytes<Elf32_Addr>();	
	eh.e_phoff = bs.ReadBytes<Elf32_Off>();	
	eh.e_shoff = bs.ReadBytes<Elf32_Off>();
	eh.e_flags = bs.ReadBytes<Elf32_Word>();	
	eh.e_ehsize = bs.ReadBytes<Elf32_Half>();
	eh.e_phentsize = bs.ReadBytes<Elf32_Half>();
	eh.e_phnum = bs.ReadBytes<Elf32_Half>();
	eh.e_shentsize = bs.ReadBytes<Elf32_Half>();
	eh.e_shnum = bs.ReadBytes<Elf32_Half>();
	eh.e_shstrndx = bs.ReadBytes<Elf32_Half>();

	if (eh.e_shstrndx == SHN_UNDEF)	
	{
		throw std::runtime_error("Error in 'FormatELF32::LoadELFHeader()': required value 'string table index' not defined");
	}
}

void FormatELF32::LoadProgramHeaders()
{
	ByteSequence bs(binDump, elfHeader.e_phoff);

	for (int i = 0; i < elfHeader.e_phnum; i++)
	{
		auto ph = Elf64_Phdr();

		ph.p_type = bs.ReadBytes<Elf32_Word>();
		ph.p_flags = bs.ReadBytes<Elf32_Word>();
		ph.p_offset = bs.ReadBytes<Elf32_Off>();
		ph.p_vaddr = bs.ReadBytes<Elf32_Addr>();
		ph.p_paddr = bs.ReadBytes<Elf32_Addr>();
		ph.p_filesz = bs.ReadBytes<Elf32_Xword>();
		ph.p_memsz = bs.ReadBytes<Elf32_Xword>();
		ph.p_align = bs.ReadBytes<Elf32_Xword>();

		programHeaders.push_back(ph);
	}
}

void FormatELF32::LoadSectionHeaders()
{
	ByteSequence bs(binDump, elfHeader.e_shoff);

	for (int i = 0; i < elfHeader.e_shnum; i++)
	{
		auto sh = Elf64_Shdr();

		sh.sh_name = bs.ReadBytes<Elf32_Word>();
		sh.sh_type = bs.ReadBytes<Elf32_Word>();
		sh.sh_flags = bs.ReadBytes<Elf32_Word>();
		sh.sh_addr = bs.ReadBytes<Elf32_Addr>();
		sh.sh_offset = bs.ReadBytes<Elf32_Off>();
		sh.sh_size = bs.ReadBytes<Elf32_Word>();
		sh.sh_link = bs.ReadBytes<Elf32_Word>();
		sh.sh_info = bs.ReadBytes<Elf32_Word>();
		sh.sh_addralign = bs.ReadBytes<Elf32_Word>();
		sh.sh_entsize = bs.ReadBytes<Elf32_Word>();

		sectionHeaders.push_back(sh);
	}
}

// ***** FormatELF64 *****

FormatELF64::FormatELF64(const std::vector<byte> * binDump)
{
	this->binDump = binDump;
	ParseBinDump();
}

void FormatELF64::LoadELFHeader()
{
	ByteSequence bs(binDump, EI_NIDENT);

	auto &eh = elfHeader;
	auto ident = LoadIdent(binDump);
	for (int i = 0; i < EI_NIDENT; i++)  
	{
		eh.e_ident[i] = ident[i];
	}

	eh.e_type = bs.ReadBytes<Elf64_Half>();	
	eh.e_machine = bs.ReadBytes<Elf64_Half>();
	eh.e_version = bs.ReadBytes<Elf64_Word>();
	eh.e_entry = bs.ReadBytes<Elf64_Addr>();
	eh.e_phoff = bs.ReadBytes<Elf64_Off>();
	eh.e_shoff = bs.ReadBytes<Elf64_Off>();
	eh.e_flags = bs.ReadBytes<Elf64_Word>();
	eh.e_ehsize = bs.ReadBytes<Elf64_Half>();
	eh.e_phentsize = bs.ReadBytes<Elf64_Half>();
	eh.e_phnum = bs.ReadBytes<Elf64_Half>();
	eh.e_shentsize = bs.ReadBytes<Elf64_Half>();
	eh.e_shnum = bs.ReadBytes<Elf64_Half>();
	eh.e_shstrndx = bs.ReadBytes<Elf64_Half>();

	if (eh.e_shstrndx == SHN_UNDEF)	
	{
		throw std::runtime_error("Error in 'FormatELF64::LoadELFHeader()': required value 'string table index' not defined");
	}

}

void FormatELF64::LoadProgramHeaders()
{
	ByteSequence bs(binDump, elfHeader.e_phoff);

	for (int i = 0; i < elfHeader.e_phnum; i++)
	{
		auto ph = Elf64_Phdr();

		ph.p_type = bs.ReadBytes<Elf64_Word>();
		ph.p_flags = bs.ReadBytes<Elf64_Word>();
		ph.p_offset = bs.ReadBytes<Elf64_Off>();
		ph.p_vaddr = bs.ReadBytes<Elf64_Addr>();
		ph.p_paddr = bs.ReadBytes<Elf64_Addr>();
		ph.p_filesz = bs.ReadBytes<Elf64_Xword>();
		ph.p_memsz = bs.ReadBytes<Elf64_Xword>();
		ph.p_align = bs.ReadBytes<Elf64_Xword>();

		programHeaders.push_back(ph);
	}
}
	
void FormatELF64::LoadSectionHeaders()
{
	ByteSequence bs(binDump, elfHeader.e_shoff);

	for (int i = 0; i < elfHeader.e_shnum; i++)
	{
		auto sh = Elf64_Shdr();

		sh.sh_name = bs.ReadBytes<Elf64_Word>();
		sh.sh_type = bs.ReadBytes<Elf64_Word>();
		sh.sh_flags = bs.ReadBytes<Elf64_Off>();
		sh.sh_addr = bs.ReadBytes<Elf64_Addr>();
		sh.sh_offset = bs.ReadBytes<Elf64_Off>();
		sh.sh_size = bs.ReadBytes<Elf64_Off>();
		sh.sh_link = bs.ReadBytes<Elf64_Word>();
		sh.sh_info = bs.ReadBytes<Elf64_Word>();
		sh.sh_addralign = bs.ReadBytes<Elf64_Off>();
		sh.sh_entsize = bs.ReadBytes<Elf64_Off>();

		sectionHeaders.push_back(sh);
	}
}

