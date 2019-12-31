cachesim: cachesim.c
	gcc -Wall -Werror -fsanitize=address -lm -o cachesim cachesim.c

clean:
	rm -f cachesim
