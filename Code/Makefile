

C = g++
CFLAG = -std=c++11

run:
	make server -C server
	gnome-terminal -- ./server/server
	python3 client/client.py 
clean:
	$(RM) inpipe outpipe
	make clean -C server
