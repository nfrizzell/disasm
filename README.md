# disasm

This is a work-in-progress disassembler. Currently disassembly of x86 executables is supported (although not yet complete), with "naive" support for x64 executables (i.e. using the same logic as for x86 executables, which leads to inaccurate output). In the future I would like to add complete support for x64 and potentially ARM.

#### Usage: 
    $ disasm [executable name]

#### File format support:
- [x] ELF
- [x] PE

#### Instruction set support:
- [x] x86
- [ ] x64
- [ ] ARM


Credit to x86asm.net (ref.x86asm.net/index.html) for providing the XML reference used.
