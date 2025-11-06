
HEADERS := $(wildcard *.h)

all : ed2k_hash.exe

%.o : %.cpp ${HEADERS}
	${CXX} -o $@ -c $< -g -O2 -Wall

%.o : %.c ${HEADERS}
	${CC} -o $@ -c $< -g -O2 -Wall

ed2k_hash.exe : ed2k_hash.o md4.o
	${CC} -o $@ $^

clean:
	rm -f *.o
	rm -f *.exe
