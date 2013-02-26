#ifndef __UTILS__H__
#define __UTILS__H__

#include <iostream>
#include <list>

class Utils
{
    public:

        Utils();
        ~Utils();

        std::string replace ( std::string orig, std::string find, std::string replace );
        std::list<std::string> split ( std::string text, std::string sep );
        std::string join ( std::list<std::string> list, std::string sep );
};

#endif
