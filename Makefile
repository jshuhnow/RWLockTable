CC=g++
IDIR=include
ODIR=obj
SDIR=src
LDIR=lib
LIBS=boost

CPPFLAGS=-O0 -g -std=c++14 -I$(IDIR) -pthread

_DEPS=LinkedList.h LLNode.h LockTable.h Record.h TransactionManager.h Random.h Constants.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))
_OBJ=main.o LinkedList.o LLNode.o LockTable.o Record.o TransactionManager.o Random.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CPPFLAGS)

run: $(OBJ)
	$(CC) -o $@ $^ $(CPPFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core
