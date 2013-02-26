#ifndef _OPTPARSER_
#define _OPTPARSER_

#include <getopt.h>
#include <iostream>

class OptParser
{
    public:

        enum Options
        {
            CONFIG,
            BPRESS,
            ABOVE_DESK,
            VBAR,
            NOFONT,
            POS,
            GROW,
            ISIZE,
            IDIST,
            NANIM,
            ZOOMF,
            JUMPF,
            DBLCLK,
            BALFA,
            FALFA,
            FILTER,
            FC,
            OFFSET,
            NORELOAD,
            TASKBAR,
            RSIZE,
            HELP = 'h',
            VERS = 'v'
        };
        OptParser ( int argc, char ** argv );
        ~OptParser();

        bool isSet ( Options opt );
        std::string getArg ( Options opt );
        char ** getArgv();

    private:
        int argc;
        char ** argv;

};


#endif /* _OPTPARSER_ */
