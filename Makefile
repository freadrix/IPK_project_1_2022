# author Anton Medvedev (xmedve04)
# version 0.01

#CFLAGS = -std=c99 -Wall -Wextra -Werror # -pedantic -lm -g
#CC = gcc
#MAIN_FILE = server.c server_query.c
#COMPILE = -o hinfosvc
#
#all:
#	$(CC) $(CFLAGS) $(MAIN_FILE) $(COMPILE)

final:
	gcc server.c -o hinfosvc -Wall -Wextra