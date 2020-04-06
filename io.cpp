
#include <queue>
#include <unistd.h>
#include "io.h"


struct processes
{
    int pid;
    string name;
};

void doWaits(vector <processes> &pids, bool printing)
{
    for (int i = 0; i < pids.size(); i++)
    {
        int status;
        status = waitpid(pids.at(i).pid, 0 , WNOHANG);

        if (printing)
        {
            string stat;
            if (status == 0) stat = "Running";
            else if ( status == -1) stat = "Finished";
            else if (status == pids.at(i).pid) stat = "Stopped";

            cout << "[" << i <<"]  " <<  stat  << " "  << "\"" << pids.at(i).name << "\""<< endl;
        }
        if (status == -1) 
        {
            pids.erase(pids.begin() + i);
            i--;
        }
        
    }
}

bool changedir(string newdr, string &previousPath)
{
    queue<string> dummy;
    queue<string> elems = split(newdr, " ", dummy, false);
    if (elems.size() <= 1) return false;
    
    
    if (pop(elems) == "cd")
    {
        
        
        if (elems.front() == "-")
        {
            chdir("../../../../../../");
            chdir(previousPath.c_str());
        }
        else
        {
            char buf[260];
            getcwd(buf, 260);
            string newstr(buf); previousPath = newstr;
            chdir(elems.front().c_str());
        }

        return true;
    }
    return false;
}

void shell()
{
    vector <processes> pids;
    string previousPath;
    char buf[260];

	while (true){
        //console coloring is used, so it might not work in all terminals
        cerr << "\033[1;31mMax's Shell \033[0m";
        getcwd(buf, 260);
        cerr << "\033[1;31m" << buf  << " $  \033[0m";
		string inputline;
        
		getline (cin, inputline); //get a line from standard input
		if (inputline == string("exit")){
			cerr << "Bye!! End of shell" ;
			break;
		}
        if (inputline == string("jobs"))
        {
            doWaits(pids, true);
            continue;
        }
    
        if (changedir(inputline, previousPath)) continue;

        doWaits(pids, false);
        lineToExecute line(inputline);

        //sleep processes
        bool sleepCommand = !line.getWaitEvent().empty();
        if (sleepCommand)
        {
            
            int pid = fork();
            processes temp;
            temp.pid = pid;
            temp.name = line.getWaitEvent();
            pids.push_back(temp);
            if (pid == 0)
                execute(line.getWaitEvent());
        }
		else if (!fork())
        {
            line.finishLine();
            exit(0);
        }
        if (!sleepCommand) wait(0);
        cerr << "\033[1;33mExecution Finished" << endl;
	}
}

int lineToExecute::finishLine()
{
    if (symbols.empty() and levels.size() == 1)
    {
        execute(levels.front());
    } 

    while (!levels.empty())
    {
        if (symbols.front() == "|" or lastComing)
            pipeSingle();
        else if(symbols.front() == "<")
            redirectFromFile();
        else if (symbols.front() == ">")
        {
            if (incomingInput)
                writeToFileFromCin();
            else
                writeToFile();
        }
        wait(0);
    }
}

void lineToExecute::pipeSingle()
{
    int fds[2];
    pipe (fds);
    if (!fork())
    {
        if (levels.size() > 1)
            dup2(fds[1], 1);
        string next = pop(levels);
        execute(next);   
    }
    else
    {
        if (levels.size() == 1)
            wait(0);    
        dup2(fds[0], 0); //copy CIN
        close(fds[1]);
    }

    pop(levels); pop(symbols);
    if (symbols.empty()) 
        lastComing = true;
}

void lineToExecute::redirectFromFile()
{
    int fds[2];
    pipe (fds);
    if (!fork())
    {
        if (levels.size() > 2)
            dup2(fds[1], 1);

        string command = pop(levels);
        string file = pop(levels);
        execute (command +  " " + file);
    }
    else
    {
        if (levels.size() == 1)
            wait(0);    
        dup2(fds[0], 0); //copy CIN
        close(fds[1]);
    }

    pop(levels); pop(levels); pop(symbols);
    incomingInput = true;
}


//execute command and send the input to the pipe
void lineToExecute::writeToFile()
{
    string command = pop(levels);
    string file = pop(levels);
    if (fork() == 0)
    {
        int fd = open(file.c_str(), O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
        dup2(fd, 1);
        execute(command);
    }
    pop(symbols);
}

//read the data from the pipe and write it to the file
void lineToExecute::writeToFileFromCin()
{
    string file = pop(levels);
    int fd = open(file.c_str(), O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
    dup2(fd, 1);
    string line;
    while (!getline(cin, line).fail() )
    {
        cout << line << endl;
    }
    pop(symbols);
}

lineToExecute::lineToExecute(string line) //constructor
{
    levels = split(line, "<>|&",symbols, true );
    oldCin = dup(0);
    oldCout = dup(1);
    if (symbols.empty()) return;
    if (symbols.front() == "&")
        waitEvent = levels.front();
}

// ***************************HELPER FUNCTIONS
queue<string> split(string str, string separator,queue<string> &savedS , bool includeQuotes)
{
    vector <string> result;
    queue <string> resQueue;
    bool quotes = false;
    result.push_back("");

    //parse through the string, it considers quotes too
    for (int i = 0; i < str.size(); i++)
    {
        char cur = str.at(i);
        if (belongsTo(separator, cur + "") and !quotes) //if we have a separator
        {
            result.push_back("");
            string t;
            t.push_back(cur);
            savedS.push(t);
        }
        else if (belongsTo("\'\"", cur + "")) //if we have a quote
        {
            quotes = !quotes;
            if (includeQuotes) result.at(result.size() - 1) += cur;
        }
        else // anything else
            result.at(result.size() - 1) += cur;
    }
    //trim final result and convert it to queue
    for (int i = 0; i < result.size(); i++)
        resQueue.push(trim(result.at(i)));

    return resQueue;
}

void execute(string str)
{
    queue <string> dummy;
    char * args[100];
    queue<string> parts = split(str, " ", dummy, false);

    int i = 0;
    while (!parts.empty())
    {
        void * block = malloc(100);
        memcpy(block, parts.front().c_str(), strlen(parts.front().c_str()) + 1);
        args[i] = (char *) block;
        parts.pop();
        i++;
    }
    args[i] = NULL;

	execvp (args [0], args);
}

string pop(queue<string> & data)
{
    if (data.empty()) return "";
    string temp = data.front();
    data.pop();
    return temp;
}

string trim(string str)
{
    if (str.empty()) return "";
    if (str.at(0) == ' ')
        str.erase(str.begin());
    if(str.at(str.size() - 1) == ' ')
        str.erase(str.begin() + str.size() - 1);
    return str;
}

bool belongsTo(string word, string token)
{
    for (int i = 0; i < word.length(); i++)
    {
        if ((word.at(i) + "") == token)
            return true;
    }
    return false;
}

void print(queue<string> contents)
{
	while (!contents.empty())
    {
        cerr << contents.front() << endl;
        contents.pop();
    }
    cerr << endl;
}
