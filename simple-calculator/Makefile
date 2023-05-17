CC=g++
cflags=-std=c++20 -fexceptions -lantlr4-runtime
SRC=src
INCLUDE=include
LLVM=`llvm-config --cxxflags --ldflags --system-libs --libs core`

calc: main.cc libparser.a
	$(CC) -o $@ $^ -L. -lparser -I$(INCLUDE) $(LLVM) $(cflags)

# listener.o:
# 	$(CC) -c listener.cc -I$(INCLUDE) $(cflags)

libparser.a:
	$(CC) -c $(SRC)/*.cpp -I$(INCLUDE) $(cflags)
	ar rcs $@ *.o
	rm *.o

clean:
	rm -f *.a calc
