final:
	gcc server.c -o hinfosvc -Wall -Wextra
test:
	gcc test.c -o test -Wall -Wextra