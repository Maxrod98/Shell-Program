
#ifndef STRINGMANIP_H
#define STRINGMANIP_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <cstring>
#include <queue>
using namespace std;

//helpers
string trim(string str);
bool belongsTo(string word, string token);
void execute(string str);
//queue<string> split(string str, string separator, bool includeQuotes, queue<string> *);
void shell();
void print(queue<string> contents);
queue<string> split(string str, string separator, queue<string> &savedSep, bool includeQuotes);
string pop(queue<string> &data);


class lineToExecute
{
    private:
    queue<string> levels;
    queue<string> symbols;
    int oldCin;
    int oldCout;
    bool incomingInput = false;
    bool lastComing = false;
    int pipeLevel;
    string waitEvent;

    public:
    lineToExecute(string line);
    string getWaitEvent()  { return waitEvent; }
    void redirectFromFile();
    void pipeSingle();
    void writeToFile();
    void writeToFileFromCin();
    int finishLine();
    
};

#endif