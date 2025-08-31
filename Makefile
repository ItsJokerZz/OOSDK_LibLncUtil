TOOLCHAIN := $(OO_PS4_TOOLCHAIN)
PROJDIR   := source
INTDIR    := build

PRX_TARGET := LibLncUtil.prx
A_TARGET   := LibLncUtil.a

PRX_ELF := $(INTDIR)/$(PROJDIR).elf

LIBS := -lc -lc++ -lz -lkernel -lSceLibcInternal \
        -lSceSystemService -lSceUserService -lSceLncUtil

CPPFILES := $(wildcard $(PROJDIR)/*.cpp)
OBJS     := $(patsubst $(PROJDIR)/%.cpp,$(INTDIR)/%.o,$(CPPFILES))

CFLAGS   := --target=x86_64-pc-freebsd12-elf -fPIC -funwind-tables -c \
             -isysroot $(TOOLCHAIN) -isystem $(TOOLCHAIN)/include \
             -Iinclude -std=c20 -D_DEFAULT_SOURCE -DFMT_HEADER_ONLY

CXXFLAGS := $(CFLAGS) -isystem $(TOOLCHAIN)/include/c++/v1 -std=c++20

LDFLAGS  := -m elf_x86_64 -pie --script $(TOOLCHAIN)/link.x --eh-frame-hdr \
             -L$(TOOLCHAIN)/lib $(LIBS) $(TOOLCHAIN)/lib/crtlib.o

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CC := clang
	CCX := clang++
	LD := ld.lld
	CDIR := linux
	ASMLR := as
endif

$(shell mkdir -p $(INTDIR))

$(PRX_TARGET): $(OBJS)
	$(LD) $(OBJS) -o $(PRX_ELF) $(LDFLAGS) -e _init
	$(TOOLCHAIN)/bin/$(CDIR)/create-fself \
		-in=$(PRX_ELF) \
		-out=$(PRX_TARGET) \
		--lib=$(PRX_ELF) \
		--paid 0x3800000000000011

$(A_TARGET): $(OBJS)
	ar rcs $(INTDIR)/$(A_TARGET) $(OBJS)

$(INTDIR)/%.o: $(PROJDIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

$(INTDIR)/%.o: $(PROJDIR)/%.cpp
	$(CCX) $(CXXFLAGS) -o $@ $<

.PHONY: all clean prx static master

all: prx static

prx: $(PRX_TARGET)

static: $(A_TARGET)

clean:
	rm -f $(PRX_TARGET) $(INTDIR)/$(PROJDIR).elf \
          $(INTDIR)/$(A_TARGET) $(OBJS)

master: clean all
