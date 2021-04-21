all: 
	gcc main.c -fsanitize=address -fsanitize=undefined -I ctl/ctl
