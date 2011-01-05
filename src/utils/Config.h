#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <iostream>
#include <list>
#include "App.h"

class Config
{
    public:
    Config();
    ~Config();
    std::string getFile();
    std::list < App * > getAppList();
};

#endif

