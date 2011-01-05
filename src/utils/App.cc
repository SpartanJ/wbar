#include "App.h"

App::App(std::string icon, std::string command, std::string title)
{
    this->icon = icon;
    this->command = command;
    this->title = title;
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

