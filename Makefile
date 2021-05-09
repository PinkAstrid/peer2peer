peer:
	cd src;gcc -c peer.c
	cd src;gcc -c peer_functions.c -lssl -lcrypto
	cd src;gcc peer_functions.o peer.o -o ../peer -lssl -lcrypto
	@echo "peer compilé"


clean:
	rm src/*.o
	rm peer
	rm central_server
	@echo "Nettoyage terminé"

central_server:
	cd src;gcc -c central_server.c
	cd src;gcc -c server_db.c
	cd src;gcc server_db.o central_server.o -o ../central_server
	@echo "central_server compilé"

all:
	@out/central_server & echo "central_server launched, PID : $$!"
	@out/peer & echo "peer launched, PID : $$!"

test_exec:
	out/test_DB

test_build:
	gcc test/test_DB.c src/server_db.c -o out/test_DB
