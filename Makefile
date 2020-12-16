CXXFLAGS=-Iinclude -g
IA_LIBS=./ia_imaging/usr/lib64/

ipa-ipu3: main.o
	$(CXX) -o $@ $^ -Wl,-rpath=$(IA_LIBS) -L$(IA_LIBS) -lia_aiq

clean:
	rm -f *.o ipa-ipu3

run: ipa-ipu3
	LD_LIBRARY_PATH=./ia_imaging/usr/lib64/ ./ipa-ipu3

debug: gdb
valgrind: ARGS=--leak-check=full

strace valgrind gdb: ipa-ipu3
	LD_LIBRARY_PATH=./ia_imaging/usr/lib64/ $@ $(ARGS) ./ipa-ipu3
