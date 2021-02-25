build:
	gcc src/central_server.c -Wall -Wextra -o out/central_server
	gcc src/peer.c -Wall -Wextra -o out/peer
	@echo "Everything have been compiled"

clean:
	rm out/*

central_server:
	out/central_server

peer:
	out/peer

all:
	@out/central_server & echo "central_server launched, PID : $$!"
	@out/peer & echo "peer launched, PID : $$!"
