#---------------------------------------------------
# Name : Abhishek Kumar
# CCID : asenthi1
# ID: 1709233
# CMPUT 275, Winter 2022
# Assignment : Part 2 Navigation System
#---------------------------------------------------

C = g++
CFLAG = -std=c++11

run:
	make server -C server
	gnome-terminal -- ./server/server
	python3 client/client.py 
clean:
	$(RM) inpipe outpipe
	make clean -C server