CC := gcc
FLAGS := -g

OUT = gameboy
SRCDIR := src/
BUILDDIR := build/

# FILES:= main memory cpu opcode
FILES:= memory.c cpu.c opcode.c main.c emulator.c

build/gameboy.out: build/main.o build/memory.o build/cpu.o build/opcode.o build/emulator.o
	${CC} ${FLAGS} $^ -o $@

build/cpu.o: src/cpu.c
	${CC} ${FLAGS} -c $< -o $@

build/opcode.o: src/opcode.c
	${CC} ${FLAGS} -c $< -o $@

build/memory.o: src/memory.c
	${CC} ${FLAGS} -c $< -o $@

build/main.o: src/main.c
	${CC} ${FLAGS} -c $< -o $@

build/emulator.o: src/emulator.c
	${CC} ${FLAGS} -c $< -o $@