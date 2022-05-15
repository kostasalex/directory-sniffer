#include "worker.hpp"
#include <queue>

#define CHILD 0

using namespace std;

/* Connects (exec)inotifywait with pipe */
void listener(const char *path, int *my_pipe);

/* Returns empty string "" in case invalid input arguments */
string getPath(const int argc,char *argv[]);

int main(int argc, char *argv[]){
    int nbytes, pid, n;

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
    pid = fork();
    if(pid == -1)
    {
        perror("forkerror\n");
        exit(1);
    }

    /*Listener process*/
    if(pid == CHILD)     
        listener(path.c_str(), my_pipe);
    
    else{  /* Manager process */                 
        char buffer[1024];
        string strBuffer, filename;

        /* Keep workers info */
        int workerIndex = -1, w, writefd;

        string fifoName = "./build/fifo", newName;
        vector<string> worker_fifo;
        vector<int> worker_pid;
        
        queue<int> workers;
    

        while(1){

            memset(buffer, 0, 1024); 

            nbytes = read(my_pipe[0], &buffer, 1024); //Reads from listener
            
            /* Append in every loop in case of interupted read from signal */
            strBuffer.append(buffer, nbytes);       

            /* filenames ends with '\n' */
            filename = popSubString(strBuffer, '\n');

            cout << endl << filename << endl;

            if(filename == "")continue;//Filename not retrieved from listener yet

            /*If no worker exist/available create new*/
            if(workers.empty()){ 
                w = ++workerIndex;

                /* Create fifo for the new worker */
                newName = fifoName + to_string(workerIndex);
                if (mkfifo(newName.c_str(), 0777) == -1){
                    perror ("fifo call failed");
                    exit(1);
                }
                worker_fifo.push_back(newName);

                /*Create worker */
                if((pid = fork()) == CHILD){
                    //Worker's(child) code
                    worker(workerIndex, newName, path);
                    break;
                }

                worker_pid.push_back(pid);
                workers.push(workerIndex);

                cout << "Im manager this is my new child: " << pid << endl;
            }
            /* If worker exist get the next available from queue */
            else{
                w = workers.back();
                workers.pop();
                cout << "Available worker: " << worker_pid[w] << 
                "with fifo name: " << worker_fifo[w];
            }

            /* Write filename to selected worker */
            if((writefd = open(worker_fifo[w].c_str(), O_WRONLY)) < 0)
                perror("client: can't open write fifo \n");
            
            n = filename.length();
            if (write(writefd, filename.c_str(), n) != n)
                perror("client: filename write error");


        }

    }

    //!Need to close all fifo before end
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