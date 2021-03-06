SUFFIXES += .d

SRC = array/slice.cpp array/ref_array.cpp array/ind_array.cpp array/array.cpp \
			array/vector.cpp array/matrix.cpp array/blas.cpp array/math.cpp \
			dataframe/column_model.cpp dataframe/column.cpp dataframe/dataframe.cpp \
			dataframe/action.cpp \
			range/range.cpp random/random.cpp \
			numeric/numeric.cpp \
			debug/debug.cpp \
			const_vector/const_vector.cpp

OBJ := $(SRC:.cpp=.o) test_suite.o
DEP := $(SRC:.cpp=.d) test_suite.d

CXXFLAGS += -DJULES_DEBUG_THROWS -std=c++14 -Wall -Wextra -Werror -pedantic -I.. -isystem../third_party/Catch/single_include
LDFLAGS += -lblas

NODEPS = clean

all: test_suite

ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
-include $(DEP)
endif

test_suite: $(OBJ)
	@echo "Linking test suite..."
	@$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.cpp
	@echo "Compiling ($<)..."
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

%.d: %.cpp
	@echo "Finding dependencies ($<)..."
	@$(CXX) $(CXXFLAGS) -MM -MT '$(patsubst %.cpp,%.o,$<)' $< -MF $@

clean:
	@echo "Cleaning test..."
	@find . -name '*.[od]' -exec rm {} \;

.PHONY: test clean
