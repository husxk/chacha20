

all: server/build compile_server client/build compile_client

server/build:
	cmake -B server/build -S server/

rserver:
	./server/build/server

compile_server:
	make -C server/build

client/build:
	cmake -B client/build -S client/

rclient:
	./client/build/client

compile_client:
	make -C client/build

clean:
	rm -rf server/build client/build

.PHONY: compile_server clean rserver compile_client rclient
