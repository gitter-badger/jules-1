SUFFIXES += .d

SRC = dataframe/column.cpp dataframe/dataframe.cpp dataframe/column_detail.cpp tutorial/naive.cpp
OBJ := $(SRC:.cpp=.o) test_suite.o
DEP := $(SRC:.cpp=.d) test_suite.d

CXXFLAGS += -std=c++14 -g -O3 -Wall -Wextra -I../include -isystem../third_party/Catch/include

NODEPS = clean

all: test_suite

ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
-include $(DEP)
endif

test: test_suite
	@echo "Running test suite..."
	@valgrind --leak-check=full --quiet ./test_suite -d yes

test_suite: $(OBJ)
	@echo "Linking test suite..."
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJ)

%.o: %.cpp
	@echo "Compiling ($<)..."
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

%.d: %.cpp
	@echo "Finding dependencies ($<)..."
	@$(CXX) $(CXXFLAGS) -MM -MT '$(patsubst %.cpp,%.o,$<)' $< -MF $@

clean:
	@echo "Cleaning test..."
	@rm -f {dataframe,tutorial}/*.{o,d} test_suite{,.o,.d}

.PHONY: test clean