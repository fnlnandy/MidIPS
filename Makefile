IPSPATCHER := ipspatcher$(EXE)

SOURCEDIR  := Source
INCLUDEDIR := Include

CXX      := g++
CXXFLAGS := -std=c++11 -Wall -Werror -O2 -I$(INCLUDEDIR)

CPPFILES := $(wildcard $(SOURCEDIR)/*.cpp)
OFILES   := $(CPPFILES:%.cpp=%.o)

all: $(IPSPATCHER)

clean:
	rm -rf *.o
	rm -f $(IPSPATCHER)

$(IPSPATCHER): $(OFILES)
	$(CXX) $(CXXFLAGS) $(OFILES) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
