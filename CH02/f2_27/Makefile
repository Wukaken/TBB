CXX = g++
CXXFLAGS = $(EXTRA_CXXFLAGS) -std=c++11 -O2
LIBS = -ltbb

MACRO = TBB_SUPPRESS_DEPRECATED_MESSAGES=1

FILE = main.cpp
OUT = out
.PHONY : all clean

all : build fixPath

build:
	source "$$TBB_ROOT/bin/tbbvars.sh" intel64 linux auto_tbbroot; source "$$PSTL_ROOT/bin/pstlvars.sh" intel64 auto_pstlroot;	$(CXX) $(CXXFLAGS) -o $(OUT) ./$(FILE) $(LIBS) -D$(MACRO); 

fixPath:
	install_name_tool -add_rpath $$TBB_ROOT/lib ./$(OUT);

clean :
	rm -f $(OUT)