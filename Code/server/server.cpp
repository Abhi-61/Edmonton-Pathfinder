/* -------------------------------------
Name : Abhishek Kumar
SID : 1709233
CCID : asenthi1
CMPUT275, Wi2022

Assignment Part 2 : Trivial Navigation System Part 2
---------------------------------------*/
#include <iostream>
#include <cassert>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unordered_map>
#include "wdigraph.h"
#include "dijkstra.h"

struct Point {
    long long lat, lon;
};

// returns the manhattan distance between two points
long long manhattan(const Point& pt1, const Point& pt2) {
  long long dLat = pt1.lat - pt2.lat, dLon = pt1.lon - pt2.lon;
  return abs(dLat) + abs(dLon);
}

// finds the id of the point that is closest to the given point "pt"
int findClosest(const Point& pt, const unordered_map<int, Point>& points) {
  pair<int, Point> best = *points.begin();

  for (const auto& check : points) {
    if (manhattan(pt, check.second) < manhattan(pt, best.second)) {
      best = check;
    }
  }
  return best.first;
}

// read the graph from the file that has the same format as the "Edmonton graph" file
void readGraph(const string& filename, WDigraph& g, unordered_map<int, Point>& points) {
  ifstream fin(filename);
  string line;

  while (getline(fin, line)) {
    // split the string around the commas, there will be 4 substrings either way
    string p[4];
    int at = 0;
    for (auto c : line) {
      if (c == ',') {
        // start new string
        ++at;
      }
      else {
        // append character to the string we are building
        p[at] += c;
      }
    }

    if (at != 3) {
      // empty line
      break;
    }

    if (p[0] == "V") {
      // new Point
      int id = stoi(p[1]);
      assert(id == stoll(p[1])); // sanity check: asserts if some id is not 32-bit
      points[id].lat = static_cast<long long>(stod(p[2])*100000);
      points[id].lon = static_cast<long long>(stod(p[3])*100000);
      g.addVertex(id);
    }
    else {
      // new directed edge
      int u = stoi(p[1]), v = stoi(p[2]);
      g.addEdge(u, v, manhattan(points[u], points[v]));
    }
  }
}
// to create and open a named pipe
int create_and_open_fifo(const char * pname, int mode) {
  // creating a fifo special file in the current working directory
  // with read-write permissions for communication with the plotter
  // both proecsses must open the fifo before they can perform
  // read and write operations on it
  if (mkfifo(pname, 0666) == -1) {
    cout << "Unable to make a fifo. Ensure that this pipe does not exist already!" << endl;
    exit(-1);
  }

  // opening the fifo for read-only or write-only access
  // a file descriptor that refers to the open file description is
  // returned
  int fd = open(pname, mode);

  if (fd == -1) {
    cout << "Error: failed on opening named pipe." << endl;
    exit(-1);
  }

  return fd;
}

// keep in mind that in part 1, the program should only handle 1 request
// in part 2, you need to listen for a new request the moment you are done
// handling one request
int main() {
    // Maximum buffer length
    const unsigned int MAX = 1024;

    WDigraph graph;
    unordered_map<int, Point> points;

    const char *inpipe = "inpipe";
    const char *outpipe = "outpipe";

    // Open the two pipes
    int in = create_and_open_fifo(inpipe, O_RDONLY);
    cout << "inpipe opened..." << endl;
    int out = create_and_open_fifo(outpipe, O_WRONLY);
    cout << "outpipe opened..." << endl;  

    // build the graph
    readGraph("server/edmonton-roads-2.0.1.txt", graph, points);

    // to read input
    string input = "";

    // In order to repeatedly take inputs from user
    bool flag = true;
    while (flag){
      Point sPoint, ePoint;

      char* buffer = new char[MAX];

      double sLat,sLon, eLat, eLon = 0;

      read(in, buffer, MAX);

      for(int i = 0; i < MAX ; i++){
        input += buffer[i];
      }
      //Deallocate the buffer and break out of loop if "Q" is prompted by user
      if(input.find("Q") != -1){
        input = "";
        delete[] buffer;
        break;
      }

      stringstream fin(input);
      fin >> sLat >> sLon >> eLat >> eLon ;

      long long coordsarray[4];
      // To take the latitudes and longitudes of the start and end points, and static cast them into long long
      coordsarray[0] = static_cast<long long>(sLat * 100000);
      coordsarray[1] = static_cast<long long>(sLon * 100000);
      coordsarray[2] = static_cast<long long>(eLat * 100000);
      coordsarray[3] = static_cast<long long>(eLon * 100000);

      sPoint.lat = coordsarray[0];
      sPoint.lon = coordsarray[1];
      ePoint.lat = coordsarray[2];
      ePoint.lon = coordsarray[3];

      // To find the points closest to the inputs on the edmontong graph file
      int start = findClosest(sPoint, points);
      int end = findClosest(ePoint, points);

      unordered_map<int, PIL> tree;
      dijkstra(graph, start, tree);

      // If no path exists
      if(tree.find(end) == tree.end()){
        write(out, "E\n", 16);
      }
      // If path exists
      else{
        vector<int> routetemp;
        vector<int> route;
        while (end != start){
          routetemp.push_back(end);
          end = tree[end].first;
        }
        routetemp.push_back(start);
        //To get the route vector in the right order from start to end
        for(int i = routetemp.size()-1; i>=0;i--){
          route.push_back(routetemp[i]);
        }

        // To write output 
        string output = "";

        for(int i = 0; i<route.size() ; i++){
          // To convert coordinates from int to string format
          string stringLat = to_string(points[route[i]].lat);
          string stringLon = to_string(points[route[i]].lon);

          //To convert coordinates into decimal format
          stringLat.insert(2,".");
          stringLon.insert(4,".");

          output += stringLat + ' ' + stringLon + '\n';
        }

        output+= "E\n";
        write(out, output.c_str(), output.size());

        output = "";
        input = "";

      }      
    }
    //closing and deleting pipes
    close(in);
    close(out);
    unlink(inpipe);
    unlink(outpipe);
    return 0;
}
