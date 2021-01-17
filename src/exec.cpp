#include <fstream>

#include "exec.h"

Executable::Executable(std::string path)
{
	filePeek = PeekFile(path);
	
	FormatType type = Format::GetFormatType(filePeek); 

	binDump = LoadExecutable(path); 

	format = Format::NewFormat(binDump, type);
	format->LoadMetadata(metadata);

	Segment seg = format->GetCodeSegment();
	arch = Arch::NewArch(seg, metadata.arch);
	std::vector<std::string> assembly = arch->TranslateToAssembly();
}

Executable::~Executable()
{
	delete filePeek;
	delete binDump;
}

std::vector<byte> * Executable::PeekFile(std::string path)
// Use for retrieving header data from potential executables to prevent
// loading a large file only to return an error if its corrupt or invalid
{
	const int minPeek = 2048; // Arbitrary value
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	if (file.fail())
	{
		throw std::runtime_error("File not found");
	}

	std::size_t peekSize = file.tellg();
	if (peekSize > minPeek)
	{
		peekSize = minPeek;
	}

	file.seekg(0);

	std::vector<byte> * filePeek = new std::vector<byte>(); 
	filePeek->resize(peekSize); // Reserve a buffer
	file.read(reinterpret_cast<char *>(filePeek->data()), peekSize);

	return filePeek;
}

std::vector<byte> * Executable::LoadExecutable(std::string path)
{
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	std::size_t fileSize = file.tellg();
	if (file.fail())
	{
		throw std::runtime_error("File not found");
	}

	file.seekg(0);

	std::vector<byte> * binDump = new std::vector<byte>(); 
	binDump->resize(fileSize); // Reserve a buffer the size of the file
	file.read(reinterpret_cast<char *>(binDump->data()), fileSize);

	return binDump;
}

