%.o:	%.c
	gcc -c -g -O0 $< -o $@

seeker:	seeker.o
	gcc $< -o seeker

clean:
	rm -fr seeker
