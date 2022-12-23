# This is the name that our final kernel executable will have.
# Change as needed.
override KERNEL := kernel.elf

# Convenience macro to reliably declare overridable command variables.
define DEFAULT_VAR =
    ifeq ($(origin $1), default)
        override $(1) := $(2)
    endif
    ifeq ($(origin $1), undefined)
        override $(1) := $(2)
    endif
endef

# It is highly recommended to use a custom built cross toolchain to build a kernel.
# We are only using "cc" as a placeholder here. It may work by using
# the host system's toolchain, but this is not guaranteed.
CC = .$(pwd)/../musl-1.2.3/obj/musl-gcc #$(eval $(call DEFAULT_VAR,CC,cc))

# Likewise, "ld" here is just a placeholder and your mileage may vary if using the
# host's "ld".
$(eval $(call DEFAULT_VAR,LD,ld))

# User controllable CFLAGS.
CFLAGS ?= -O2 -g -fpermissive -w -pipe
CPPFLAGS = -std=gnu++11

ASMC = nasm
ASMFLAGS = 

# User controllable linker flags. We set none by default.
LDFLAGS ?=

# Internal C flags that should not be changed by the user.
override INTERNALCFLAGS :=							\
	-I.		             							\
	-fno-exceptions									\
	-fno-rtti			 							\
	-ffreestanding       							\
	-fno-stack-protector 							\
	-fno-stack-check     							\
	-fno-pic             							\
	-fno-pie             							\
	-mabi=sysv           							\
	-mno-80387           							\
	-mno-mmx             							\
	-mno-3dnow           							\
	-mno-sse             							\
	-mno-sse2            							\
	-mno-red-zone        							\
	-mcmodel=kernel      							\
	-MMD

# Internal linker flags that should not be changed by the user.
override INTERNALLDFLAGS :=    \
	-Tlinker.ld            \
	-nostdlib              \
	-zmax-page-size=0x1000 \
	-static

# Use find to glob all *.cpp files in the directory and extract the object names.
override CFILES := $(shell find ./ -type f -name '*.c')
override CPPFILES := $(shell find ./ -type f -name '*.cpp')
override ASMFILES := $(shell find ./ -type f -name '*.asm')
override OBJ := $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) $(ASMFILES:.asm=_asm.o)
override HEADER_DEPS := $(CFILES:.c=.d) $(CPPFILES:.cpp=.d)

# Default target.
.PHONY: all
all: $(KERNEL)

limine.h:
	curl https://raw.githubusercontent.com/limine-bootloader/limine/trunk/limine.h -o $@

# Link rules for the final kernel executable.
$(KERNEL): $(OBJ)
	mkdir -p ../root
	$(LD) $(OBJ) $(LDFLAGS) $(INTERNALLDFLAGS) -o ../root/kernel.elf

# Compilation rules for *.cpp files.
-include $(HEADER_DEPS)
%_asm.o: %.asm
	$(ASMC) $(ASMFLAGS) -f elf64 $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(INTERNALCFLAGS) -c $< -o $@

%.o: %.cpp limine.h
	$(CC) $(CFLAGS) $(INTERNALCFLAGS) $(CPPFLAGS) -c $< -o $@

# Remove object files and the final executable.
.PHONY: clean
clean:
	rm -rf $(KERNEL) $(OBJ) $(HEADER_DEPS)

.PHONY: distclean
distclean: clean
	rm -f limine.h
