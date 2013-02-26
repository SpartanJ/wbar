#ifndef __RUN__H__
#define __RUN__H__

#include <iostream>
#include <list>

class Run
{
    public:

        Run();
        ~Run();

        int getPID();
        char ** getArg ( int & argc );
        bool restart ( std::string command );
        bool start ( std::list<std::string> list );
        bool start ( std::string command );
        void stop();
};

#endif
