TOOLCHAIN      := $(OO_PS4_TOOLCHAIN)
PROJDIR        := source
INTDIR         := build

TARGET         := libLncUtil.prx
TARGETSTATIC   := libLncUtil.a

LIBS           := -lc -lkernel -lSceLibcInternal -lSceSysmodule \
                  -lSceSystemService -lSceUserService -lSceLncUtil

CFILES         := $(wildcard $(PROJDIR)/*.c)
HFILES         := $(wildcard $(PROJDIR)/*.h)
OBJS           := $(patsubst $(PROJDIR)/%.c, $(INTDIR)/%.o, $(CFILES))

CFLAGS         := --target=x86_64-pc-freebsd12-elf -fPIC -funwind-tables -c \
                  -isysroot $(TOOLCHAIN) -isystem $(TOOLCHAIN)/include \
                  -Iinclude -std=c17 -D_DEFAULT_SOURCE

LDFLAGS        := -m elf_x86_64 -pie --script $(TOOLCHAIN)/link.x --eh-frame-hdr \
                  -L$(TOOLCHAIN)/lib $(LIBS) $(TOOLCHAIN)/lib/crtlib.o

$(TARGET): $(INTDIR) $(OBJS)
	$(LD) $(OBJS) -o $(INTDIR)/$(PROJDIR).elf $(LDFLAGS) -e _init
	$(TOOLCHAIN)/bin/$(CDIR)/create-fself -in=$(INTDIR)/$(PROJDIR).elf \
	-out=$(INTDIR)/$(PROJDIR).oelf --lib=$(TARGET) --paid 0x3800000000000011

$(TARGETSTATIC): $(OBJS)
	$(AR) --format=bsd rcs $(INTDIR)/$(TARGETSTATIC) $(OBJS)

$(INTDIR)/%.o: $(PROJDIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<


_unused := $(shell mkdir -p $(INTDIR))
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
	CC := clang
	CCX := clang++
	LD := ld.lld
	AR := llvm-ar
	CDIR := linux
	ASMLR := as
endif

.PHONY: clean all master target

clean:
	rm -f $(INTDIR)/$(PROJDIR).oelf $(OBJS) $(TARGET) $(INTDIR)/$(TARGETSTATIC) $(INTDIR)/$(PROJDIR).elf

target: $(TARGET) $(TARGETSTATIC)

all: target

master: clean all

install: all
	@echo Copying...
	@cp -frv $(PROJDIR)/*.h $(OO_PS4_TOOLCHAIN)/include/
	@cp -frv $(INTDIR)/$(TARGETSTATIC) $(OO_PS4_TOOLCHAIN)/lib
	@echo Done!
