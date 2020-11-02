CXX = g++
CXXFLAGS = $(EXTRA_CXXFLAGS) -std=c++11 -O2
LIBS = -ltbb

MACRO = TBB_SUPPRESS_DEPRECATED_MESSAGES=1

FILE = main.cpp
OUT = out
.PHONY : all clean

all :
	source $(TBB_ROOT)/bin/tbbvars.sh intel64 linux auto_tbbroot;
	$(CXX) $(CXXFLAGS) -o $(OUT) ./$(FILE) $(LIBS) -D$(MACRO)

clean :
	rm -f $(OUT)