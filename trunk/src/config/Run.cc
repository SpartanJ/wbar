#include "Run.h"
#include "Utils.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fstream>
#include <dirent.h>
#include <config.h>

Run::Run()
{

}

Run::~Run()
{

}

int Run::getPID()
{
    int pid = -1;
    FILE * process;

    process = popen("pidof "PACKAGE_NAME, "r");

    if (process)
    {
        char spid[50];
        fgets(spid, sizeof(spid), process);
        pclose(process);

        pid = atoi(spid);
    }

    return pid;
}

char ** Run::getArg(int &argc)
{
    std::list<std::string> list;
    argc = 0;
    int pid = Run::getPID();
    char **argv = NULL;

    if (pid > 0)
    {
        char data[512];
        std::string line;

        sprintf (data, "/proc/%d/cmdline", pid);
        std::ifstream file (data);

        if (file.is_open())
        {
            char c[2];

            while(!file.eof())
            {
                file.read (c,1);
                c[1] = 0;
                line += (c[0] != 0 ? c:" ");
            }

            file.close();
	    Utils util;
            list = util.split(line, " ");

            argc = list.size();

            if (argc > 1)
            {
               std::list<std::string>::iterator ac;
               argv = new char * [argc + 1];
               int i = 0;

               for (ac = list.begin();ac != list.end();ac++, i++)
               {
                  argv[i] = strdup((*ac).c_str());
               }

               argv[argc] = NULL;
            }

        }
    }

    return argv;
}

bool Run::restart(std::string command)
{
    Run::stop();
    Utils util;
    return Run::start(util.split( command, " " ));
}

bool Run::start(std::list<std::string> list)
{
    if (list.size() == 0) return false;

    std::string args;

    std::list<std::string>::iterator it;

    for (it = list.begin(), it++;it != list.end();it++)
    {
        args += " ";
        args += (*it);
    }
    
    return start(args);
}

bool Run::start(std::string command)
{
    system ((PACKAGE_NAME" " + command + " &").c_str());
    return Run::getPID() > 0;
}

void Run::stop()
{
    if (Run::getPID() > 0)
    {
	system ("killall "PACKAGE_NAME);
    }
}

