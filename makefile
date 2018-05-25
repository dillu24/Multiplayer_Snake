all: servermake clientmake

servermake: connection.c server.c game.c
	gcc -o server connection.c server.c game.c -lncurses -lpthread -I.

clientmake: client.c connection.c
	gcc -o client client.c connection.c -lncurses -lpthread -I.
