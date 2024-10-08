CXX=g++
CXXFLAGS = -Wall -std=c++17
BUILD_DIR = build

app: $(BUILD_DIR)/app

$(BUILD_DIR)/app: $(BUILD_DIR)/main.o
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/app $(BUILD_DIR)/main.o

$(BUILD_DIR)/main.o: main.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c main.cpp -o $(BUILD_DIR)/main.o

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)