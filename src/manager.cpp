#include "worker.hpp"
#include <queue>


#define CHILD 0

using namespace std;

/* Connects (exec)inotifywait with pipe */
void listener(const char *path, int *my_pipe);

/* Returns empty string "" in case invalid input arguments */
string getPath(const int argc,char *argv[]);


int workersReady, listener_pid;
vector<int> worker_pid;     //Map index with pid 


void handler(int signal)
{  
    if(signal == SIGINT){

        cout << "Manager: Killing listener proccess with pid: "<< listener_pid << endl;
        kill(listener_pid, SIGINT);
        cout << "Manager: Killing worker processes.." << endl;
        while(!worker_pid.empty()){
            kill(worker_pid.back(), SIGCONT);
            kill(worker_pid.back(), SIGINT);
            worker_pid.pop_back();
        }
        exit(0);
    }
    else
       workersReady++;
}


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
    if((pid = fork()) == CHILD)     
        listener(path.c_str(), my_pipe);
    
    else{  /* Manager process */                 
        char buffer[1024];
        listener_pid = pid;
        string writePath = "./files/", strBuffer, filename;

        /* Signal handling for user(SIGINT) and workers(SIGCHLD)*/
        struct sigaction action;
        workersReady = 0;

	    memset(&action, 0, sizeof(action));
        action.sa_handler = handler;
        action.sa_flags = SA_RESTART;
        sigaction(SIGCHLD, &action, NULL);
        sigaction(SIGINT, &action, NULL);


        /* Keep workers info */
        int workerIndex = -1, w, writefd, status;

        string fifoName = "./build/fifo", newName;
        vector<string> worker_fifo; //Map index with fifo name 
        queue<int> workers;
    


        while(1){

            memset(buffer, 0, 1024); 

            nbytes = read(my_pipe[0], &buffer, 1024); //Reads from listener

            /* Append in every loop in case of interupted read from signal */
            strBuffer.append(buffer, nbytes);       

            /* filenames ends with '\n' */
            filename = popSubString(strBuffer, '\n');
            cout << "Manager: got file " << filename << " from listener" << endl;

            if(filename == "")continue;//Filename not retrieved from listener yet


            /* If at least one worker send SIGTSTP, 
            add all available workers to the queue*/
            if(workersReady){
                workersReady = 0;
                while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED))){
                    for(int i = 0; i <= workerIndex; i++){
                        if(worker_pid[i] == pid){
                            workers.push(i);
                            break;
                        }
                    }
                }
            }

            
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

                /*Create new worker */
                if((pid = fork()) == CHILD){
                    //Worker's(child) code
                    worker(workerIndex, newName, path, writePath);
                    break;
                }
                worker_pid.push_back(pid);
                cout << "Manager: new worker created with pid: " << pid << endl;
            }
            /* If worker exist get the next available from queue */
            else{
                w = workers.front();
                workers.pop();
                cout  << "Manager: available worker for the job: " << worker_pid[w] << endl;
                kill(worker_pid[w], SIGCONT); //"wake" the child
            }

            /* Write filename to selected worker */

            if((writefd = open(worker_fifo[w].c_str(), O_WRONLY)) < 0)
                perror("manager: can't open fifo \n");

            n = filename.length();
            if (write(writefd, filename.c_str(), n) != n)
                perror("manager: filename write error");

        }
    }
    exit(0);
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