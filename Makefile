CFLAGS=-std=gnu99 -Wall -Wextra -pedantic -pthread -g -Werror 
FILES=main.c
PROJ=hinfosvc

all : $(PROJ)

$(PROJ) : $(FILES)
		gcc $(CFLAGS) -o $(PROJ) $(FILES)

clean :
	rm -f *.o $(PROJ) 