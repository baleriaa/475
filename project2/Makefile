CXX = g++-13
CXXFLAGS = -g -Wall -fopenmp

# Files
SOURCES = main.cpp barrier.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = simulation

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile each source file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
