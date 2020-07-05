#include "risc-v/load.h"

#include <fcntl.h>
#include <sys/stat.h>

#include <cassert>
#include <cstdio>

#include "risc-v/elf.h"

namespace risc_v {

using namespace risc_v::elf;

namespace {

#define DUMP(x) do {printf("  " #x " = %u(0x%x)\n", (uint32_t)x, (uint32_t)x);} while(0);

void DumpEhdr(Elf32_Ehdr *ehdr) {
    int i;
    printf("  ehdr->e_ident = ");
    for (i = 0; i < EI_NIDENT; i++) {
        printf("%02x ", ehdr->e_ident[i]);
    }
    printf("\n");
    DUMP(ehdr->e_type);
    DUMP(ehdr->e_machine);
    DUMP(ehdr->e_version);
    DUMP(ehdr->e_entry);
    DUMP(ehdr->e_phoff);
    DUMP(ehdr->e_shoff);
    DUMP(ehdr->e_flags);
    DUMP(ehdr->e_ehsize);
    DUMP(ehdr->e_phentsize);
    DUMP(ehdr->e_phnum);
    DUMP(ehdr->e_shentsize);
    DUMP(ehdr->e_shnum);
    DUMP(ehdr->e_shstrndx);
    printf("\n");
}

void DumpPhdr(Elf32_Phdr *phdr, int e_phnum) {
    int i;
    for (i = 0; i < e_phnum; i++, phdr++) {
        DUMP(phdr->p_type);
        DUMP(phdr->p_flags);
        DUMP(phdr->p_offset);
        DUMP(phdr->p_vaddr);
        DUMP(phdr->p_paddr);
        DUMP(phdr->p_filesz);
        DUMP(phdr->p_memsz);
        DUMP(phdr->p_align);
        printf("\n");
    }
    printf("\n");
}

void DumpShdr(Elf32_Shdr *shdr, int e_shnum) {
    int i;
    for (i = 0; i < e_shnum; i++, shdr++) {
        DUMP(shdr->sh_name);
        DUMP(shdr->sh_type);
        DUMP(shdr->sh_flags);
        DUMP(shdr->sh_addr);
        DUMP(shdr->sh_offset);
        DUMP(shdr->sh_size);
        DUMP(shdr->sh_link);
        DUMP(shdr->sh_info);
        DUMP(shdr->sh_addralign);
        DUMP(shdr->sh_entsize);
        printf("\n");
    }
    printf("\n");
}

}  // namespace

void LoadE(Memory *mem, const char *fname) {
    Elf32_Ehdr *ehdr;
    Elf32_Phdr *phdr;
    Elf32_Shdr *shdr;
    int fd;
    FILE *fp;
    struct stat stbuf;
    int i, j, offset, addr;

    fd = open(fname, O_RDONLY);
    assert(fd != -1);
    fp = fdopen(fd, "rb");
    assert(fp);

    fstat(fd, &stbuf);
    unsigned char buf[stbuf.st_size];
    assert(fread(buf, 1, sizeof(buf), fp) == (unsigned long)stbuf.st_size);
    fclose(fp);

    printf("Elf file header\n");
    ehdr = (Elf32_Ehdr *)buf;
    DumpEhdr(ehdr);

    printf("Program header\n");
    phdr = (Elf32_Phdr *)(&buf[ehdr->e_phoff]);
    DumpPhdr(phdr, ehdr->e_phnum);

    for (i = 0; i < ehdr->e_phnum; i++, phdr++) {
        offset = phdr->p_offset;
        addr = phdr->p_vaddr;
        for (j = 0; j < phdr->p_memsz; j++, offset++, addr++) {
            mem->PutMem(addr, buf[offset], Memory::Size::kByte);
        }
    }

    printf("Section header\n");
    shdr = (Elf32_Shdr *)(&buf[ehdr->e_shoff]);
    DumpShdr(shdr, ehdr->e_shnum);

#if 0
    for (i = 0; i < ehdr->e_shnum; i++, shdr++) {
        offset = shdr->sh_offset;
        addr = shdr->sh_addr;
        for (j = 0; j < shdr->sh_size; j++, offset++, addr++) {
            mem->PutMem(addr, buf[offset], Memory::Size::kByte);
        }
    }
#endif
}

}  // namespace risc_v
