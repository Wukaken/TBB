CXX = g++
CXXFLAGS = $(EXTRA_CXXFLAGS) -std=c++11 -O2
LIBS = -ltbb

MACRO = TBB_SUPPRESS_DEPRECATED_MESSAGES=1

MAINFILE = main.cpp
BASEFUNCFILE = baseFunc.cpp
MAINOUT = main.o
BASEFUNCOUT = baseFunc.o
OUT = out
.PHONY : all clean

all : baseFunc main merge fixPath

main:
	source "$$TBB_ROOT/bin/tbbvars.sh" intel64 linux auto_tbbroot; source "$$PSTL_ROOT/bin/pstlvars.sh" intel64 auto_pstlroot;	$(CXX) $(CXXFLAGS) -c -o $(MAINOUT) ./$(MAINFILE) $(LIBS) -D$(MACRO); 

baseFunc:
	source "$$TBB_ROOT/bin/tbbvars.sh" intel64 linux auto_tbbroot; source "$$PSTL_ROOT/bin/pstlvars.sh" intel64 auto_pstlroot;	$(CXX) $(CXXFLAGS) -c -o $(BASEFUNCOUT) ./$(BASEFUNCFILE) $(LIBS) -D$(MACRO); 

merge:
	source "$$TBB_ROOT/bin/tbbvars.sh" intel64 linux auto_tbbroot; source "$$PSTL_ROOT/bin/pstlvars.sh" intel64 auto_pstlroot;	$(CXX) $(CXXFLAGS) -o $(OUT) ./$(MAINOUT) ./$(BASEFUNCOUT) $(LIBS) -D$(MACRO); 

fixPath:
	install_name_tool -add_rpath $$TBB_ROOT/lib ./$(OUT);

clean :
	rm -f $(OUT)