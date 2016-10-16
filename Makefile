%.o:	%.c
	gcc -DDEBUG -c -g -O0 $< -o $@

seeker:	seeker.o
	gcc -DDEBUG $< -o seeker

clean:
	rm -fr seeker
