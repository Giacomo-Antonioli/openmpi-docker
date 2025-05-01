# Makefile for hybridgpu

# Compiler and flags
CXX      = mpic++
CXXFLAGS = -fopenmp -O2 -std=c++11

# Target executable and source file(s)
TARGET   = hybridgpu
SRCS     = hybridgpu.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
