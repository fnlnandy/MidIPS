IPSPATCHER := ipspatcher$(EXE)

SOURCEDIR  := Source
INCLUDEDIR := Include
BUILDDIR   := Build

CXX      := g++
CXXFLAGS := -std=c++11 -Wall -Werror -O2 -I$(INCLUDEDIR)

CPPFILES := $(wildcard $(SOURCEDIR)/*.cpp)
OFILES   := $(CPPFILES:$(SOURCEDIR)/%.cpp=$(BUILDDIR)/%.o)

all: mkdirs $(IPSPATCHER)

clean:
	rm -rf $(BUILDDIR)
	rm -f $(IPSPATCHER)

mkdirs:
	mkdir -p $(BUILDDIR)

$(IPSPATCHER): $(OFILES)
	$(CXX) $(CXXFLAGS) $(OFILES) -o $@

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
