#include "App.h"

App::App(std::string icon, std::string command, std::string title, 
    unsigned long winid)
{
    this->icon = icon;
    this->command = command;
    this->title = title;
    this->winid = winid;
}

App::~App()
{
}

std::string App::getIconName()
{
    return icon;
}

std::string App::getCommand()
{
    return command;
}

std::string App::getTitle()
{
    return title;
}

unsigned long App::getWinid()
{
    return winid;
}

