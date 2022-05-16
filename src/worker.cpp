#include "worker.hpp"
#include <vector>

using namespace std;
#define MAXBUFF 1024

struct result{
    string url;
    int counter = 1;
};

int readfifo;
string fifo;

/* SIGINT from manager */
void handlerWorker(int signal)
{  
    cout << "Worker with pid: " << getpid() <<  " terminated from manager.. \n";
    close(readfifo);
    remove(fifo.c_str());
    exit(0);
}


/* Add new url the vector, or count it if allready exist */
void urlCounter(vector<struct result> &urls, string url){

    struct result temp;
    bool found = false;
    
    int size = urls.size();

    for(int i = 0; i < size; i++){
        if(urls[i].url == url){
            found = true;
            urls[i].counter++;
            break;
        }
    }

    if(found == false){
        temp.url = url;
        urls.push_back(temp);
    }
    return;
}



string popSubString(string& my_string, char lastChar){
    string result = ""; 
    size_t pos = my_string.find(lastChar);

    if(pos!= string::npos){         //If position found
        result = my_string.substr(0, pos+1);
        result.pop_back();          //Remove lastchar ('\n')
        my_string.erase(0,pos+1);
    }

    return result;
}

/*Returns url and erases url + all chars before. If url not exist returns ""*/
string popUrl(string& my_string){
    size_t pos;
    string result = "";

    /* Find url and erase previous chars */
    pos = my_string.find("http://");     
    my_string.erase(0, pos);

    if(pos!= string::npos){
        result = popSubString(my_string,' ');
    }

    return result;
}



void cleanUrl(string& url){
    size_t pos;
    string http = "http://", www = "www.";

    /* Erase "http://" if exists */
    pos = url.find(http);    
    if(pos != string::npos)
        url.erase(0, http.length());

    /* Erase "www" if exists */
    pos = url.find(www);    
    if(pos != string::npos)
        url.erase(0, www.length());

    /* Erase after '/' */
    pos = url.find('/');    
    if(pos != string::npos)
        url.erase(pos, url.length() - pos);

    return;
}



void worker(int i, string fifoname, string readPath, string writePath){

    int nbytes, readfd, writefd;
    char buffer[MAXBUFF];
    string filename, path_filename, strBuffer, url, writeUrl;

    fifo = fifoname;

    
    /* Signal handling from manager (SIGINT)*/
    struct sigaction action;

    memset(&action, 0, sizeof(action));
    action.sa_handler = handlerWorker;
    action.sa_flags = SA_RESTART;
    sigaction(SIGINT, &action, NULL);

    vector <struct result> urls;
    struct result temp;

    /* Open fifo */
    if((readfifo = open(fifoname.c_str(), O_RDONLY)) < 0)
        perror("worker: can't open read fifo \n");
    

    while(1){
        cout << i << endl;
        /* Read from fifo */
        memset(buffer, 0, MAXBUFF); 
 
        read(readfifo, buffer, MAXBUFF);

        /* Open the file to read the urls */
        cout << "Im worker " << i << endl;

        filename = buffer;
        path_filename = readPath + filename;

        //*Testing
        cout << "the file name that you gave me is " << filename << endl;

        if((readfd = open(path_filename.c_str(), O_RDONLY)) < 0)
        {
            perror("worker: can't open read fifo \n");
            exit(1);
        }

        /* Create new file to save the urls */
        filename = filename + ".out";
        path_filename = writePath + filename;
      
        if((writefd = open(path_filename.c_str(), O_WRONLY | O_CREAT, 0777)) < 0)
        {
            perror("worker: can't open/create file.out \n");
            exit(1);
        }


        /* Read from the given file */
        memset(buffer, 0, MAXBUFF); 

        strBuffer = "";

        while((nbytes = read(readfd, buffer, MAXBUFF)) > 0){
            strBuffer.append(buffer, nbytes); 

            while((url = popUrl(strBuffer)) != ""){
                cleanUrl(url);
                urlCounter(urls, url);
            }
        }
        close(readfd);

        while(!urls.empty()){
            temp = urls.back();
            writeUrl = temp.url + " " + to_string(temp.counter) + "\n";
            //*testing
            write(writefd, writeUrl.c_str(), writeUrl.length());
            urls.pop_back();
        }
        close(writefd);
        
        /* Stop and wait signal from manager */
        raise(SIGTSTP);
    }

    exit(0);

}