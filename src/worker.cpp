#include "worker.hpp"

using namespace std;
#define MAXBUFF 1024

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

void worker(int i, string fifoname, string path){
    int readfd, n;
    char buff[MAXBUFF];
    string filename;


    if((readfd = open(fifoname.c_str(), O_RDONLY)) < 0)
    {
        perror("client: can't open read fifo \n");
    }

    while(1){

        memset(buff, 0, MAXBUFF); 

        if ((n= read(readfd, buff, MAXBUFF)) <= 0) {
            perror("server: filename read error ");
        }

        cout << "Im worker " << i << endl;
        cout << "my fifo is " << fifoname << endl;

        filename = buff;

        cout << "the file name that you gave me is " << filename << endl;
    }

    close(readfd);

}