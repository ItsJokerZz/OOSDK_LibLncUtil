TOOLCHAIN := $(OO_PS4_TOOLCHAIN)
PROJDIR   := source
INTDIR    := build

PRX_TARGET := LibLncUtil.prx
SO_TARGET  := LibLncUtil.so

LIBS      := -lc -lc++ -lz -lkernel -lSceLibcInternal \
             -lSceSystemService -lSceUserService -lSceLncUtil

CPPFILES  := $(wildcard $(PROJDIR)/*.cpp)
OBJS      := $(patsubst $(PROJDIR)/%.cpp, $(INTDIR)/%.o, $(CPPFILES))

CFLAGS    := --target=x86_64-pc-freebsd12-elf -fPIC -funwind-tables -c \
             -isysroot $(TOOLCHAIN) -isystem $(TOOLCHAIN)/include \
             -Iinclude -std=c20 -D_DEFAULT_SOURCE -DFMT_HEADER_ONLY

CXXFLAGS  := $(CFLAGS) -isystem $(TOOLCHAIN)/include/c++/v1 -std=c++20 -Iinclude -DFMT_HEADER_ONLY

LDFLAGS_PRX := -m elf_x86_64 -pie --script $(TOOLCHAIN)/link.x --eh-frame-hdr \
                -L$(TOOLCHAIN)/lib $(LIBS) $(TOOLCHAIN)/lib/crtlib.o

LDFLAGS_SO := -shared --export-dynamic -L$(TOOLCHAIN)/lib $(LIBS)

$(PRX_TARGET): $(INTDIR) $(OBJS)
	$(LD) $(OBJS) -o $(INTDIR)/$(PROJDIR).elf $(LDFLAGS_PRX) -e _init
	$(TOOLCHAIN)/bin/$(CDIR)/create-fself -in=$(INTDIR)/$(PROJDIR).elf \
	-out=$(INTDIR)/$(PROJDIR).oelf --lib=$(PRX_TARGET) --paid 0x3800000000000011

$(SO_TARGET): $(OBJS)
	$(LD) -o $(INTDIR)/$@ $(OBJS) $(LDFLAGS_SO)

$(INTDIR)/%.o: $(PROJDIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

$(INTDIR)/%.o: $(PROJDIR)/%.cpp
	$(CCX) $(CXXFLAGS) -o $@ $<

_unused := $(shell mkdir -p $(INTDIR))
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
	CC := clang
	CCX := clang++
	LD := ld.lld
	CDIR := linux
	ASMLR := as
endif

.PHONY: clean all target

clean:
	rm -f $(INTDIR)/$(PROJDIR).oelf $(OBJS) $(PRX_TARGET) $(SO_TARGET) $(INTDIR)/$(PROJDIR).elf

target: $(PRX_TARGET) $(SO_TARGET)

all: target

master: clean all
