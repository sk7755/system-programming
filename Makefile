CC = gcc
TARGET = 20151607.out
CFLAG = -Wall
OBJECTS = 20151607.o argument.o memory.o opcode.o shell.o exception.o hash.o assemble.o debug.o linkloader.o simulator.o 

.SUFFIXES: .c .o

all : $(TARGET)

$(TARGET) : $(OBJECTS)
			  $(CC) $(CFLAG) -o $@ $(OBJECTS)

clean :
	rm -f $(OBJECTS) $(TARGET)

argument.o : argument.c argument.h
memory.o : memory.c memory.h
opcode.o : opcode.c opcode.h
shell.o : shell.c shell.h
exception.o : exception.c exception.h
hash.o : hash.c hash.h
assemble.o : assemble.c assemble.h
debug.o : debug.c debug.h
linkloader.o : linkloader.c linkloader.h
simulator.o : simulator.c simulator.h
20151607.o : 20151607.c main.h argument.h
