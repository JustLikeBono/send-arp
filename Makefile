all: send-arp

send-arp: send-arp.o main.o
	gcc -o send-arp send-arp.o main.o

main.o: send-arp.h main.c

send-arp.o: send-arp.h send-arp.c

clean:
	rm -f send-arp.*
	rm -f *.o