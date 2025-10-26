TARGET = allPossibleImages
OBJS = main.o MatrixView.o

CXXFLAGS = -Wall -std=c++17
LDFLAGS = -lbe

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(TARGET) *.o
