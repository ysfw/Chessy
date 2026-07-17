CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Isrc

SRCS = src/main.cpp src/game.cpp src/board.cpp src/helpers.cpp $(wildcard src/pieces/*.cpp)
OBJS = $(SRCS:.cpp=.o)
EXEC = chessy

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)

.PHONY: all clean
