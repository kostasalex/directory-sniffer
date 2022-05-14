#include <iostream>
#include <cstring>
#include <filesystem>
#include <unistd.h>
#include <sys/inotify.h>
#include <fcntl.h>

using namespace std;

/* Connects (exec)inotifywait with pipe */
void listener(const char *path, int *my_pipe);

/* Returns empty string "" in case invalid input arguments */
string getPath(const int argc,char *argv[]);

/*Pops a sub string ending with "last char", if not exist returns ""*/
string popSubString(string &my_string, char lastChar);

int main(int argc, char *argv[]){

    int i, nbytes;

    /* Get the path to be monitored */
    string path;
    path = getPath(argc,argv);
    if(path == ""){      //Error case wrong input
        cout << "Usage: ./sniffer [-p path]  or ./sniffer  (for current path)" 
        << endl;
        return -1;
    }
    
    /* Create pipe for listener */
    int my_pipe[2];
    if(pipe(my_pipe) == -1)
    {
        perror("Error creating pipe\n");
        exit(1);
    }

    /* Create child process to run listener's code*/
    pid_t listener_id;
    listener_id = fork();
    if(listener_id == -1)
    {
        perror("forkerror\n");
        exit(1);
    }

    /*Listener process*/
    if(listener_id == 0)     
        listener(path.c_str(), my_pipe);
    
    else{  /* Manager process */                 
        char buffer[1024];
        string strBuffer, filename;
        while(1){

            memset(buffer, 0, 1024); 

            nbytes = read(my_pipe[0], &buffer, 1024); //Reads from listener
            
            /* Append in every loop in case of interupted read from signal */
            strBuffer.append(buffer, nbytes);       
            cout << "Read " << nbytes << "bytes\n";
            cout << strBuffer << endl;
            /* filenames ends with '\n' */
            filename = popSubString(strBuffer, '\n');
            cout << "filename is: " << filename << endl;
        }

    }
    return 0;

}

string getPath(const int argc, char *argv[]){

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


void listener(const char *path, int *my_pipe){

    close(my_pipe[0]);               // listener doesn't read
    dup2(my_pipe[1], STDOUT_FILENO); // redirect stdout

    execlp("inotifywait", "inotifywait", path, "-m","-e","create", "-e", \
    "moved_to", "--format", "%f" , NULL);

    /*Exec Call failed*/
    perror("internal: exec error");
    exit(1);

}

string popSubString(string& my_string, char lastChar){
    string result = ""; 
    size_t pos = my_string.find(lastChar);

    if(pos!= string::npos){
        result = my_string.substr(0, pos+1);
        result.pop_back();          //Remove lastchar ('\n')
        my_string.erase(0,pos+1);
    }

    return result;
}