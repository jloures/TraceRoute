# Makefile

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

TARGET = traceroute
SRCS = main.cpp TraceRoute.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@rm -f $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(TARGET) $(OBJS)