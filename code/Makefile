source = czl_vm.c czl_mm.c czl_opt.c czl_lib.c czl_paser.c czl_shell.c czl_com.c test.c
objects = czl_vm.o czl_mm.o czl_opt.o czl_lib.o czl_paser.o czl_shell.o test.o

a.o:$(objects)
	gcc $(source) -o czl -lm -lpthread -O2

.PHONY:clean
clean:
	rm czl $(objects)
