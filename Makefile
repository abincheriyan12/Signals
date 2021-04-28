signals: signals.c
	gcc -o signals signals.c -pthread -lm -Wall -Werror