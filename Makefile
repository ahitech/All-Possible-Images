TARGET = allPossibleImages
OBJS = main.o MatrixView.o SettingsWindow.o

CXXFLAGS = -Wall -std=c++17
LDFLAGS = -lbe -ltranslation -llocalestub

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(TARGET) *.o
