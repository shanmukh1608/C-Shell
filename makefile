all:
	gcc -Wall -Wextra *.c -o shell -g -w
	touch .history.txt
