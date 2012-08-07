# Macros
 CC = gcc217
# CC = gcc217m

 CCFLAGS = -g
# CCFLAGS = -DNDEBUG
# CCFLAGS = -DNDEBUG -O3

# Dependency rules for non-file targets
all: ish
clobber: clean
	rm -f *~ \#*\# core
clean:
	rm -f ish*.o exec*.o parse*.o lexi*.o hist*.o dynarray*.o

# Dependency rules for file targets
ish: ish.o exec.o parse.o lexi.o hist.o dynarray.o
	$(CC) $(CCFLAGS) ish.o exec.o parse.o lexi.o hist.o dynarray.o -o ish

ish.o: ish.c exec.h parse.h lexi.h hist.h dynarray.h
	$(CC) $(CCFLAGS) -c ish.c
exec.o: exec.c parse.h lexi.h dynarray.h
	$(CC) $(CCFLAGS) -c exec.c
parse.o: parse.c lexi.h dynarray.h
	$(CC) $(CCFLAGS) -c parse.c
lexi.o: lexi.c dynarray.h
	$(CC) $(CCFLAGS) -c lexi.c
hist.o: hist.c dynarray.h
	$(CC) $(CCFLAGS) -c hist.c
dynarray.o: dynarray.c
	$(CC) $(CCFLAGS) -c dynarray.c



