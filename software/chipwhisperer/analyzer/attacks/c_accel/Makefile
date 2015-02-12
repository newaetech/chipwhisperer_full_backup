
module = cpa
old_libs = CPAProgressive.o AESModels.o
dependency_libs = 

all:
	g++ -march=native -fPIC -o CPAProgressive.o -c CPAProgressive.cpp
	g++ -march=native -fPIC -o AESModels.o -c AESModels.cpp
	g++ -shared -o lib${module}.so -Wl,--soname=lib${module}.so   -Wl,--whole-archive ${old_libs} -Wl,--no-whole-archive ${dependency_libs}

