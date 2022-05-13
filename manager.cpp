#include <iostream>
#include <cstring>
#include <filesystem>
#include <unistd.h>
#include <sys/inotify.h>

using namespace std;

/* Returns empty string "" in case invalid input arguments */
string getPath(int argc, char *argv[]);


int main(int argc, char *argv[]){

    /* Get the path to be monitored */
    string path;
    path = getPath(argc,argv);
    if(path == ""){      //Error case wrong input
        cout << "Usage: ./sniffer [-p path]  or ./sniffer  (for current path)" 
        << endl;
        return -1;
    }

    return 0;

}

string getPath(int argc, char *argv[]){

    string path = "";
    
    switch(argc){
        case 1:
            path = "./";
            break;
        case 3: //2nd argument should be "[-p" and 3rd = "path]"
            string arg2 = argv[1];
            string arg3 = argv[2];
            if(arg2 == "[-p" && arg3.back() == ']'){
                arg3.pop_back();
                path = arg3;
                break;
            }
    }

    return path;
}
