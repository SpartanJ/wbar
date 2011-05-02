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
    void setFile( std::string configFile );
    std::list < App * > getAppList();
    private:
    std::string configFile;
};

#endif

