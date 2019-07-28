 source = czl_vm.c czl_mm.c czl_opt.c czl_lib.c czl_paser.c czl_shell.c czl_com.c czl_tcp.c czl_udp.c czl_http.c czl_ws.c czl_pcre.c test.c
objects = czl_vm.o czl_mm.o czl_opt.o czl_lib.o czl_paser.o czl_shell.o czl_com.o czl_tcp.o czl_udp.o czl_http.o czl_ws.o czl_pcre.o test.o

a.o:$(objects)
	gcc $(source) -o czl -lm -lrt -lpthread -lpcre -O2

.PHONY:clean
clean:
	rm czl $(objects)
