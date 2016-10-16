%.o:	%.c
	gcc -c -g -O0 $< -o $@

seeker:	seeker.o
	gcc -g -O0 $< -o seeker

clean:
	rm -fr seeker
	rm -fr *.o
