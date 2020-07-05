#include "risc-v/memory.h"

#include <cstdio>
#include <cstring>

namespace risc_v {

Memory::Memory(): pair_ptr_(0) {
    memset(mem_, 0, sizeof(mem_) / sizeof(mem_[0][0]));
    memset(pair_, 0, sizeof(pair_) / sizeof(pair_[0]));
}

int Memory::Search(int addr) {
    int i;

    if (pair_ptr_ < 0) {  // init
        pair_ptr_ = 0;
        pair_[pair_ptr_] = addr & 0xffff0000L;
        return pair_ptr_;
    }

    for (i = 0; i <= pair_ptr_; i++) {
        if (((pair_[i] ^ addr) & 0xffff0000L) == 0)
            break;
    }
    if (i <= pair_ptr_) {  // match
        return i;
    }

    if ((pair_ptr_ + 1) < kMaxPageNum) {  // allocate
        pair_ptr_++;
        pair_[pair_ptr_] = addr & 0xffff0000L;
        return pair_ptr_;
    }

    printf("memory.cc:out of memory at 0x%08x\n", addr);
    return -1;
}

int Memory::GetWord(int addr) {
    int i;
    int c0, c1, c2, c3;

    addr &= 0xfffffffcL;
    i = Search(addr);
    if (i >= 0) {
        addr &= 0xffff;
        c0 = mem_[i][addr + 0] & 0xff;
        c1 = mem_[i][addr + 1] & 0xff;
        c2 = mem_[i][addr + 2] & 0xff;
        c3 = mem_[i][addr + 3] & 0xff;
        return (c3 << 24) | (c2 << 16) | (c1 << 8) | (c0);
    }
    return 0;
}

void Memory::PutWord(int addr, int data) {
    int i;
    int c0, c1, c2, c3;

    addr &= 0xfffffffcL;
    i = Search(addr);
    if (i >= 0) {
        addr &= 0xffff;
        c0 = (data      ) & 0xff;
        c1 = (data >>  8) & 0xff;
        c2 = (data >> 16) & 0xff;
        c3 = (data >> 24) & 0xff;
        mem_[i][addr + 0] = c0;
        mem_[i][addr + 1] = c1;
        mem_[i][addr + 2] = c2;
        mem_[i][addr + 3] = c3;
    }
}

int Memory::GetMem(int addr, Size size) {
    int d;

    d = GetWord(addr);

    if (size == Size::kWord)
        return d;

    if (size == Size::kHWord) {
        if (addr & 2)  // upper
            return (d >> 16) & 0xffff;
        else  // lower
            return  d & 0xffff;
    }

    if (size == Size::kByte) {
        switch (addr & 3) {
        case 0:
            return d & 0xff;
        case 1:
            return (d >> 8) & 0xff;
        case 2:
            return (d >> 16) & 0xff;
        case 3:
            return (d >> 24) & 0xff;
        }
    }

    return 0;
}

void Memory::PutMem(int addr, int value, Size size) {
    int d;

    if (size == Size::kWord) {
        PutWord(addr, value);
        return;
    }

    d = GetWord(addr);

    if (size == Size::kHWord) {
        if (addr & 2) {  // upper
            PutWord(addr, ((value & 0xffff) << 16) | (d & 0xffff));
            return;
        } else {  // lower
            PutWord(addr, (value & 0xffff) | (d & 0xffff0000));
            return;
        }
    }

    if (size == Size::kByte) {
        switch (addr & 3) {
        case 0:
            PutWord(addr, (value & 0x000000ff) | (d & 0xffffff00));
            return;
        case 1:
            PutWord(addr, ((value << 8) & 0x0000ff00) | (d & 0xffff00ff));
            return;
        case 2:
            PutWord(addr, ((value << 16) & 0x00ff0000) | (d & 0xff00ffff));
            return;
        case 3:
            PutWord(addr, ((value << 24) & 0xff000000) | (d & 0x00ffffff));
            return;
        }
    }
}

void Memory::Dump(unsigned int start, unsigned int end, Unit unit) {
  unsigned int  m_start;
  unsigned int  m_end;
  unsigned int  a,c;
  int    width,ainc;
  int    data;
  unsigned char   asc[16];
  unsigned char   d0,d1,d2,d3;

  if(unit==Unit::kByte){
    width=16;
    ainc =1;
  }
  else if(unit==Unit::kHWord){
    width=8;
    ainc =2;
  }
  else {
    unit = Unit::kWord;
    width=4;
    ainc =4;
  }
  m_start = start&0xfffffffcL;
  m_end   = end  &0xfffffffcL;
  for(a=m_start,c=0;a<m_end;a += 4){
    if(c==0) printf("%08x: ",a);
    data = GetWord(a);
    asc[4*c+0]=d0=data&0xff;
    asc[4*c+1]=d1=(data>>8)&0xff;
    asc[4*c+2]=d2=(data>>16)&0xff;
    asc[4*c+3]=d3=(data>>24)&0xff;
    switch(unit){
    case Unit::kByte:
      printf("%02x %02x %02x %02x  ",d0,d1,d2,d3);
      break;
    case Unit::kHWord:
      printf("%04x %04x ",(d1<<8)|d0,(d3<<8)|d2);
      break;
    case Unit::kWord:
      printf("%08x ",data);
      break;
    }
    asc[4*c+0]=((d0>=' ')&&(d0<='~')) ? d0 : '.';
    asc[4*c+1]=((d1>=' ')&&(d1<='~')) ? d1 : '.';
    asc[4*c+2]=((d2>=' ')&&(d2<='~')) ? d2 : '.';
    asc[4*c+3]=((d3>=' ')&&(d3<='~')) ? d3 : '.';
    c++;
    if(c==4){
      int     i;
      c=0;
      for(i=0;i<16;i++) printf("%c",asc[i]);
      printf("\n");
    }
  }
  if(c!=0){
    int i;
    while(c<4){
      switch(unit){
      case Unit::kByte:
    printf("             ");
	break;
      case Unit::kHWord:
	printf("          ");
	break;
      case Unit::kWord:
	printf("         ");
	break;
      }
      asc[4*c+0]=' ';
      asc[4*c+1]=' ';
      asc[4*c+2]=' ';
      asc[4*c+3]=' ';
      c++;
    }
    for(i=0;i<16;i++) printf("%c",asc[i]);
    printf("\n");
  }
}

}  // namespace risc_v
