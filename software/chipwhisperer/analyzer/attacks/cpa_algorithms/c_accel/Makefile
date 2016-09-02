
module = cpa
old_libs = CPAProgressive.o AESModels.o
dependency_libs = 

all:
	g++ -march=native -fPIC -o CPAProgressive.o -c CPAProgressive.cpp
	g++ -march=native -fPIC -o AESModels.o -c AESModels.cpp
	g++ -shared -o lib${module}.so -Wl,-install_name,lib${module}.so   ${old_libs}

#-install_name changed from --soname
