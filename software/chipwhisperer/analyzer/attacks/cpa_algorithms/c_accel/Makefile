
module = cpa
old_libs = CPAProgressive.o AESModels.o
dependency_libs = 

all:
	g++ -march=native -fPIC -o CPAProgressive.o -c CPAProgressive.cpp
	g++ -march=native -fPIC -o AESModels.o -c AESModels.cpp
	g++ -shared -o lib${module}.so -Wl,--soname,lib${module}.so   ${old_libs}

# --soname flag used to be called -install_name 
#Not sure what version this change is needed for, if error check that first
