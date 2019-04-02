CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Werror -g
LDFLAGS  =

FLEX  = flex
BISON = bison

OUTPUT = compile

SOURCES = src/parser.cc src/scanner.cc src/driver.cc src/main.cc src/util.cc $(wildcard src/ast/*.cc)
OBJECTS = $(SOURCES:.cc=.o)

all: $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	$(CXX) $^ $(LDFLAGS) -o $@

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.cc %.hh: %.yy
	$(BISON) -o $*.cc $<

%.cc: %.ll
	$(FLEX) -o $@ $<

src/main.o: src/parser.hh
src/driver.o: src/parser.hh
src/scanner.o: src/parser.hh

clean:
	rm -f $(OUTPUT) $(OBJECTS) src/parser.hh src/parser.cc src/scanner.cc src/location.hh
