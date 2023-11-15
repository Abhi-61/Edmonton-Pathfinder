# ---------------------------------------------------
# Name : Abhishek Kumar
# CCID : asenthi1
# ID	 : 1709233
# CMPUT 275, Winter 2022

# Assignment : Part 2 Navigation System
# ---------------------------------------------------

C = g++ 
CFLAG = -std=c++11
EX = server
OBJS = server.o digraph.o dijkstra.o

server: server.o digraph.o dijkstra.o
	g++ $(OBJS) -o $(EX) -Wall $(CFLAG)

server.o: server.cpp
	g++ server.cpp -c

digraph.o:	digraph.cpp
	g++ digraph.cpp -c

dijkstra.o: dijkstra.cpp
	g++ dijkstra.cpp -c

clean:
	rm -f $(OBJS) $(EX)
