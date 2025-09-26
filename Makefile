test:
	morloc make test.loc
	./nexus test

clean:
	rm -f *.o *.gch a.out pool* nexus*
