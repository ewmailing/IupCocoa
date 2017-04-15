CC=gcc
CFLAGS=-O -g -Wall
LIBS=-liupimglib -liup
INCS=-I../../../include
LDIR=-L../../../lib/${TEC_UNAME}

all: example2_1 example2_2 example2_3 example2_4 example2_5 example3_1 example3_2 example3_3 example3_4 example3_5 example3_6 example3_7 example3_8

example2_1: example2_1.o 
	$(CC) $(CFLAGS) -o example2_1 example2_1.o $(LIBS) $(LDIR)
example2_1.o: example2_1.c
	$(CC) -c example2_1.c $(INCS)

example2_2: example2_2.o 
	$(CC) $(CFLAGS) -o example2_2 example2_2.o $(LIBS) $(LDIR)
example2_2.o: example2_2.c
	$(CC) -c example2_2.c $(INCS)

example2_3: example2_3.o 
	$(CC) $(CFLAGS) -o example2_3 example2_3.o $(LIBS) $(LDIR)
example2_3.o: example2_3.c
	$(CC) -c example2_3.c $(INCS)

example2_4: example2_4.o 
	$(CC) $(CFLAGS) -o example2_4 example2_4.o $(LIBS) $(LDIR)
example2_4.o: example2_4.c
	$(CC) -c example2_4.c $(INCS)

example2_5: example2_5.o 
	$(CC) $(CFLAGS) -o example2_5 example2_5.o $(LIBS) $(LDIR)
example2_5.o: example2_5.c
	$(CC) -c example2_5.c $(INCS)

example3_1: example3_1.o 
	$(CC) $(CFLAGS) -o example3_1 example3_1.o $(LIBS) $(LDIR)
example3_1.o: example3_1.c
	$(CC) -c example3_1.c $(INCS)

example3_2: example3_2.o 
	$(CC) $(CFLAGS) -o example3_2 example3_2.o $(LIBS) $(LDIR)
example3_2.o: example3_2.c
	$(CC) -c example3_2.c $(INCS)

example3_3: example3_3.o 
	$(CC) $(CFLAGS) -o example3_3 example3_3.o $(LIBS) $(LDIR)
example3_3.o: example3_3.c
	$(CC) -c example3_3.c $(INCS)

example3_4: example3_4.o 
	$(CC) $(CFLAGS) -o example3_4 example3_4.o $(LIBS) $(LDIR)
example3_4.o: example3_4.c 
	$(CC) -c example3_4.c $(INCS)

example3_5: example3_5.o 
	$(CC) $(CFLAGS) -o example3_5 example3_5.o $(LIBS) $(LDIR)
example3_5.o: example3_5.c
	$(CC) -c example3_5.c $(INCS)

example3_6: example3_6.o 
	$(CC) $(CFLAGS) -o example3_6 example3_6.o $(LIBS) $(LDIR)
example3_6.o: example3_6.c
	$(CC) -c example3_6.c $(INCS)

example3_7: example3_7.o 
	$(CC) $(CFLAGS) -o example3_7 example3_7.o $(LIBS) $(LDIR)
example3_7.o: example3_7.c
	$(CC) -c example3_7.c $(INCS)

example3_8: example3_8.o 
	$(CC) $(CFLAGS) -o example3_8 example3_8.o $(LIBS) $(LDIR)
example3_8.o: example3_8.c
	$(CC) -c example3_8.c $(INCS)



ifeq ($(OS), Windows_NT)
  TECMAKE_CMD = $(MAKE) --no-print-directory -f ../../../tecmakewin.mak TEC_UNAME=$(TEC_UNAME)
else
  TECMAKE_CMD = $(MAKE) --no-print-directory -f ../../../tecmake.mak
endif

simple_paint:
	@$(TECMAKE_CMD) 
