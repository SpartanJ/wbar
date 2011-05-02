#ifndef _OPTPARSER_
#define _OPTPARSER_

#include <getopt.h>
#include <iostream>

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
    HELP = 'h',
    VERS = 'v'
};

class OptParser
{
    public:

	OptParser(int argc, char **argv);
        ~OptParser();

	bool isSet(Options opt);
	std::string getArg(Options opt);

    private:
    int argc;
    char **argv;

};


#endif /* _OPTPARSER_ */
