CC=gcc
CFLAGS= -lm
DEPS = QTree.h
OBJ = QTree.o barneshut.o
TEST_nParticles = 300
TEST_nIterations = 200

all: barneshut

%.o: %.c $(DEPS)
	@echo Compiling $<...
	$(CC) -c -o $@ $< $(CFLAGS)
	@echo

barneshut: $(OBJ)
	@echo Compiling executable...
	$(CC) -o $@ $^ $(CFLAGS)
	@echo

test: barneshut
	./barneshut $(TEST_nParticles) $(TEST_nIterations) 0.0 1 1
	@echo
	@echo To change number of particles and iterations change TEST_nParticles and TEST_nIterations in Makefile

plot: particles.dat
	python plot.py

clean:
	\rm -f *.o barneshut barneshut.o QTree.o 