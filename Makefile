VERSION=$(shell git tag | sed 's/.*\([0-9]\+\.[0-9]\+\.[0-9]\+\)/\1/' | sort -nt . | tail -1)

BIN = html2mark
TEST_BIN = $(BIN)_test
SOURCES = $(wildcard src/*.cpp)
APP_SOURCES = $(wildcard *.cpp)
TEST_SOURCES = $(wildcard test/*.cpp)
OBJ = $(addprefix tmp/,$(SOURCES:.cpp=.o))
APP_OBJ = $(addprefix tmp/,$(APP_SOURCES:.cpp=.o))
TEST_OBJ = $(addprefix tmp/,$(TEST_SOURCES:.cpp=.o))
LIBS = -lchthon2
# -Wpadded -Wuseless-cast -Wvarargs 
WARNINGS = -pedantic -Werror -Wall -Wextra -Wformat=2 -Wmissing-include-dirs -Wswitch-default -Wswitch-enum -Wuninitialized -Wunused -Wfloat-equal -Wundef -Wno-endif-labels -Wshadow -Wcast-qual -Wcast-align -Wconversion -Wsign-conversion -Wlogical-op -Wmissing-declarations -Wno-multichar -Wredundant-decls -Wunreachable-code -Winline -Winvalid-pch -Wvla -Wdouble-promotion -Wzero-as-null-pointer-constant -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn
CXXFLAGS = -MD -MP -std=c++0x $(WARNINGS) -Wno-sign-compare

all: $(BIN)

check: test
	cppcheck --enable=all --template='{file}:{line}: {severity}: {message} ({id})' .

test: $(TEST_BIN)
	./$(TEST_BIN) $(TESTS)

deb: $(BIN)
	@debpackage.py \
		html2mark \
		-v $(VERSION) \
		--maintainer 'umi041 <umi0451@gmail.com>' \
		--bin $(BIN) \
		--build-dir tmp \
		--dest-dir . \
		--description 'Utility to convert HTML to Markdown.'

$(BIN): $(APP_OBJ) $(OBJ)
	$(CXX) -o $@ $^ $(LIBS)

$(TEST_BIN): $(OBJ) $(TEST_OBJ)
	$(CXX) -o $@ $^ $(LIBS)

tmp/%.o: %.cpp
	@echo Compiling $<...
	@$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean Makefile check test

clean:
	$(RM) -rf tmp/* $(TEST_BIN) $(BIN)

$(shell mkdir -p tmp)
$(shell mkdir -p tmp/src)
$(shell mkdir -p tmp/test)
-include $(OBJ:%.o=%.d)
-include $(APP_OBJ:%.o=%.d)
-include $(TEST_OBJ:%.o=%.d)

