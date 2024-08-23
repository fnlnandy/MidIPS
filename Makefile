MIDIPS := midips$(EXE)

SOURCEDIR  := Source
INCLUDEDIR := Include
BUILDDIR   := Build

CXX      := g++
CXXFLAGS := -std=c++11 -Wall -Werror -O2 -I$(INCLUDEDIR)

CPPFILES := $(wildcard $(SOURCEDIR)/*.cpp)
OFILES   := $(CPPFILES:$(SOURCEDIR)/%.cpp=$(BUILDDIR)/%.o)

all: mkdirs $(MIDIPS)

clean:
	rm -rf $(BUILDDIR)
	rm -f $(MIDIPS)

mkdirs:
	mkdir -p $(BUILDDIR)

$(MIDIPS): $(OFILES)
	$(CXX) $(CXXFLAGS) $(OFILES) -o $@

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
