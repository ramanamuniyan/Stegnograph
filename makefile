obj =$(patsubst %.c,%.o,$(wildcard *.c))
stego.exe : $(obj)
	gcc -o stego.exe $(obj)

clean:
	rm *.exe *.o
