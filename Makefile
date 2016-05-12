CC = g++
EXE = mcu
CFLAGS += -std=c++11 -pthread

MCU_SRC    = main.cpp mcu.cpp worker.cpp writer.cpp queuemt.cpp

MCU_OBJ = $(MCU_SRC:.cpp=.o)
OBJ = $(MCU_OBJ)

all: $(OBJ) $(EXE)

mcu: $(MCU_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(OBJ): %.o: %.cpp
	$(CC) -c $(CFLAGS) $^

clean:
	rm -f $(EXE) *.o
