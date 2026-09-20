CROSS   := x86_64-eucalypt-elf

CC      := $(CROSS)-gcc
CXX     := $(CROSS)-g++
AR      := $(CROSS)-ar

CFLAGS  := -O2 -ffreestanding -fno-stack-protector -fno-stack-check \
           -fno-asynchronous-unwind-tables -mno-red-zone -m64 -mcmodel=small -Wall -Wextra

MLIBC := $(abspath ../eucalypt-distro/eucalypt-mlibc/build/install/src/usr/local)
MLIBC_INC := $(MLIBC)/include
MLIBC_LIB := $(MLIBC)/lib

CRTBEGIN := $(shell $(CC) -print-file-name=crtbegin.o)
CRTEND   := $(shell $(CC) -print-file-name=crtend.o)

LDFLAGS := -nostdlib -static -z max-page-size=0x1000 -z noexecstack

LDSCRIPT := src/linker.ld
BINDIR   := src/bin
ARCHIVE  := userspace.tar

LIBS := $(MLIBC_LIB)/libc.a $(MLIBC_LIB)/libssp_nonshared.a $(MLIBC_LIB)/libssp.a \
$(MLIBC_LIB)/libpthread.a $(MLIBC_LIB)/libm.a $(MLIBC_LIB)/libutil.a

SRCS := $(filter-out src/dso_stub.c,$(wildcard src/*.c))
BINS := $(addprefix $(BINDIR)/,$(patsubst src/%.c,%,$(SRCS)))

.PHONY: all clean usr create_archive

all: $(BINS) usr

$(BINDIR)/%: src/%.c $(LDSCRIPT) $(LIBS) $(MLIBC_LIB)/crt1.o
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -I$(MLIBC_INC) -c $< -o $@.o
	$(CC) $(LDFLAGS) -T $(LDSCRIPT) \
		$(MLIBC_LIB)/crt1.o $(CRTBEGIN) $@.o \
		$(LIBS) $(CRTEND) \
		-o $@
	strip --strip-debug $@
	rm -f $@.o

clean:
	rm -rf $(BINDIR) src/usr $(ARCHIVE)

usr:
	@mkdir -p src/usr/bin src/usr/lib src/usr/include src/usr/src
	cp $(MLIBC_LIB)/libc.a $(MLIBC_LIB)/libm.a $(MLIBC_LIB)/libssp.a \
	$(MLIBC_LIB)/libssp_nonshared.a $(MLIBC_LIB)/libpthread.a \
	$(MLIBC_LIB)/libutil.a $(MLIBC_LIB)/crt1.o src/usr/lib/
	$(CC) $(CFLAGS) -c src/dso_stub.c -o src/usr/lib/crti.o
	printf '.text\n' | $(CROSS)-as -o src/usr/lib/crtn.o
	cp -r $(MLIBC_INC)/. src/usr/include/

create_archive:
	tar -cf $(ARCHIVE) -C src usr bin