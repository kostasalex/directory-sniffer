#include "worker.hpp"
#include <vector>

using namespace std;
#define MAXBUFF 1024

struct result{
    string url;
    int counter = 1;
};

void urlCounter(vector<struct result> &urls, string url){

    struct result temp;
    bool found = false;
    
    int size = urls.size();
    cout << "...." << url <<"...." << endl;
    for(int i = 0; i < size; i++){
        cout << "i: " << i << endl;
        if(urls[i].url == url){
            cout << "same: ";
            cout << urls[i].url << " - " << url << endl;
            found = true;
            urls[i].counter++;
            break;
        }
        else{
            cout << "NOT same: ";
            cout << urls[i].url << " - " << url << endl;
        }
    }

    if(found == false){
        cout << "adding new: " << url << endl;
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
    int readfd, writefd, nbytes;
    char buffer[MAXBUFF];
    string filename, path_filename, strBuffer, url, writeUrl;

    vector <struct result> urls;
    struct result temp;

    /* Open fifo */
    if((readfd = open(fifoname.c_str(), O_RDONLY)) < 0)
    {
        perror("worker: can't open read fifo \n");
    }

    while(1){

        /* Read from fifo */
        memset(buffer, 0, MAXBUFF); 

        if ((nbytes= read(readfd, buffer, MAXBUFF)) <= 0) {
            perror("worker: fifo read error ");
            exit(0);
        }

        //*Testing
        /* Open the file to read the urls */
        cout << "Im worker " << i << endl;
        cout << "my fifo is " << fifoname << endl;

        filename = buffer;
        path_filename = readPath + filename;

        //*Testing
        cout << "the file name that you gave me is " << filename << endl;
        cout << "the file name with path " << path_filename << endl;


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
            perror("worker: can't open read fifo \n");
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

        while(!urls.empty()){
            temp = urls.back();
            sleep(1);
            writeUrl = temp.url + " " + to_string(temp.counter) + "\n";
            //*testing
            //cout << writeUrl << endl;
            write(writefd, writeUrl.c_str(), writeUrl.length());
            urls.pop_back();
        }

    }

    close(readfd);

    exit(0);

}