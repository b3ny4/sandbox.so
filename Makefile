
# I am not proud of this Makefile. Unless you are seeking for pain (or valuable information you don't get otherwise) just ignore it's content.
# Read at your own risk! Pain ahead

.PHONY: normal sandboxed exploit clean all_calls

normal: cat cat_exploit1 cat_exploit2 cat_exploit3 cat_exploit4
	./cat
	./cat_exploit1
	./cat_exploit2
	./cat_exploit3
	./cat_exploit4

all_calls: sandbox.so all_calls.c
	gcc -o all_calls all_calls.c
	@echo
	@echo Testing all calls
	@echo ==========================
	export LD_PRELOAD=$(CURDIR)/sandbox.so &&\
	./all_calls flag.txt || test 0==0

sandboxed: cat sandbox.so
	@echo
	@echo Testing cat without sandbox
	@echo ==========================
	./cat flag.txt
	@echo
	@echo Testing cat with sandbox
	@echo ==========================
	export LD_PRELOAD=$(CURDIR)/sandbox.so &&\
	./cat flag.txt || test 0==0

exploit: cat_exploit1 cat_exploit2 cat_exploit3 cat_exploit4 sandbox.so
	@echo
	@echo Testing exploit1 with sandbox
	@echo ==========================
	export LD_PRELOAD=$(CURDIR)/sandbox.so &&\
	./cat_exploit1 flag.txt || test 0==0
	@echo
	@echo Testing exploit2 with sandbox
	@echo ==========================
	export LD_PRELOAD=$(CURDIR)/sandbox.so &&\
	./cat_exploit2 flag.txt || test 0==0
	@echo
	@echo Testing exploit3 with sandbox
	@echo ==========================
	export LD_PRELOAD=$(CURDIR)/sandbox.so &&\
	./cat_exploit3 flag.txt || test 0==0
	@echo
	@echo Testing exploit4 with sandbox
	@echo ==========================
	export LD_PRELOAD=$(CURDIR)/sandbox.so &&\
	./cat_exploit4 flag.txt || test 0==0


clean:
	rm -rf *.o
	rm -rf *.so
	rm -rf cat
	rm -rf cat_exploit1
	rm -rf cat_exploit2
	rm -rf cat_exploit3
	rm -rf cat_exploit4
	rm -rf all_calls
	rm -rf whitelist
	rm -rf blacklist

cat: cat.c
	gcc -o cat cat.c

cat_exploit1: cat_exploit1.c
	gcc -o cat_exploit1 cat_exploit1.c -ldl

cat_exploit2: cat_exploit2.c
	gcc -o cat_exploit2 cat_exploit2.c

cat_exploit3: cat_exploit3.c
	gcc -o cat_exploit3 cat_exploit3.c

cat_exploit4: cat.c
	gcc -o cat_exploit4 cat.c -static

sandbox.so: sandbox.o
	gcc -shared -o sandbox.so sandbox.o -ldl
	echo "$(shell pwd)/flag.txt" > blacklist
	echo "$(shell pwd)" > whitelist

sandbox.o: sandbox.c
	gcc -fPIC -c -o sandbox.o sandbox.c

