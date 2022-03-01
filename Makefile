final:
	gcc server.c -o hinfosvc -Wall -Wextra
test:
	gcc test.c -o testing -Wall -Wextra