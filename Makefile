ifeq ($(MAKECMDGOALS),linux)
CFLAGS= -c -Wall  -Wextra  -pedantic -std=c++11
CC=g++
SOURCES=logger.cpp test_linux.cpp
EXECUTABLE=test_linux
LDFLAGS= -pthread
endif

ifeq ($(MAKECMDGOALS),windows)
CC=i586-mingw32msvc-g++
CFLAGS= -c -Wall  -Wextra  -pedantic
SOURCES=logger.cpp test_windows.cpp
EXECUTABLE=test_windows
LDFLAGS=
endif

#LDFLAGS= -pthread

OBJECTS=$(SOURCES:.cpp=.o)


linux: $(SOURCES) $(EXECUTABLE)
	./test_linux

windows: $(SOURCES) $(EXECUTABLE)
	mv $(EXECUTABLE) $(EXECUTABLE).exe
	wine test_windows.exe

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ 

clean:
	rm -rf *.o test_linux *.exe
