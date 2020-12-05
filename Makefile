all:
	gcc -std=c99 -Wall -pedantic-errors -Werror -DNDEBUG priority_queue.c main.c -o app
	# gcc main.c priority_queue.c -o app -std=c99

# run commands:
# make   <-- complies code into app exe
# ./app  <-- runs the code