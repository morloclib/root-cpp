clean:
	rm -f *.o *.gch a.out pool* nexus*

test:
	morloc make test.loc
	./nexus test
