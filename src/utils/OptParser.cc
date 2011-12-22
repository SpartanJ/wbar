#include "OptParser.h"

static struct option long_options[] =
{
    {"config", 1, 0, 0},
    {"bpress", 0, 0, 0},
    {"above-desk", 0, 0, 0},
    {"vbar", 0, 0, 0},
    {"nofont", 0, 0, 0},
    {"pos", 1, 0, 0},
    {"grow", 0, 0, 0},
    {"isize", 1, 0, 0},
    {"idist", 1, 0, 0},
    {"nanim", 1, 0, 0},
    {"zoomf", 1, 0, 0},
    {"jumpf", 1, 0, 0},
    {"dblclk", 1, 0, 0},
    {"balfa", 1, 0, 0},
    {"falfa", 1, 0, 0},
    {"filter", 1, 0, 0},
    {"fc", 1, 0, 0},
    {"offset", 1, 0, 0 },
    {"noreload", 0, 0, 0 },
    {"taskbar", 0, 0, 0 },
    {"help", 0, 0, HELP},
    {"version", 0, 0, VERS},
    {0, 0, 0, 0}
};

OptParser::OptParser(int argc, char **argv)
{
    this->argc = argc;
    this->argv = argv;
}

OptParser::~OptParser()
{
}

bool OptParser::isSet(Options opt)
{
    if (argc <= 1) return false;
    int c;

    optarg = NULL;
    optind = opterr = optopt = 0;

    while (1)
    {
        int option_index = 0;

        c = getopt_long_only(argc, argv, "hcv", long_options, &option_index);
        if (c == -1) break;
        if (option_index == (int)opt) return true;
        if (c == (int) HELP && opt == HELP) return true;
        if (c == (int) VERS && opt == VERS) return true;
    }

    return false;
}

std::string OptParser::getArg(Options opt)
{
    if (argc <= 1) return "";

    int c;

    optarg = NULL;
    optind = opterr = optopt = 0;

    while (1)
    {
        int option_index = 0;

        c = getopt_long_only(argc, argv, "hcv", long_options, &option_index);
        if (c == -1) break;
        if (!optarg) continue;
        if (option_index == (int)opt) return optarg;
    }

    return "";
}

char ** OptParser::getArgv ()
{
    return this->argv;
}
