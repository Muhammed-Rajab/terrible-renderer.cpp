CXX=g++
CXXFLAGS = -Iinclude -Wall -std=c++17
BUILD_DIR = build

app: $(BUILD_DIR)/app

$(BUILD_DIR)/app: $(BUILD_DIR)/main.o $(BUILD_DIR)/hsl.o $(BUILD_DIR)/renderer.o
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/app $(BUILD_DIR)/main.o $(BUILD_DIR)/hsl.o $(BUILD_DIR)/renderer.o

$(BUILD_DIR)/main.o: main.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c main.cpp -o $(BUILD_DIR)/main.o

$(BUILD_DIR)/hsl.o: src/hsl.cpp | $(BUILD_DIR) include/hsl.h
	$(CXX) $(CXXFLAGS) -c src/hsl.cpp -o $(BUILD_DIR)/hsl.o

$(BUILD_DIR)/renderer.o: src/renderer.cpp | $(BUILD_DIR) include/renderer.h
	$(CXX) $(CXXFLAGS) -c src/renderer.cpp -o $(BUILD_DIR)/renderer.o

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)