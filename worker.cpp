#include "worker.hpp"

using namespace std;

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
    int i;
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