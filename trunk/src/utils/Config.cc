#include "Config.h"
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
#include <regex.h>
#include <iostream>
#include "i18n.h"

Config::Config()
{
    configFile = "";
}

Config::~Config()
{
}

std::string Config::getFile()
{
    if (configFile != "")
    {
        return configFile;
    }

    std::string homeDir = getenv("HOME");
    std::string configFile = homeDir + "/.wbar";

    struct stat cfg_stat;

    if (stat(configFile.c_str(), &cfg_stat))
    {
        std::string command = "";
        command += "cp ";
        command += DEFAULT_CONFIGDIR"/wbar.cfg ";
        command += configFile;

        if (system(command.c_str()) != 0)
        {
            std::cout << _("Error copy file: ") << DEFAULT_CONFIGDIR"/wbar.cfg" << std::endl;
        }
    }

    return configFile;
}

std::list < App * > Config::getAppList()
{
    std::list < App * > list;
    std::ifstream file(getFile().c_str());

    if (!file.is_open())
    {
        return list;
    }

    regex_t r_icon, r_title, r_command;
    regmatch_t pmatch[2];

    regcomp(&r_icon, "^[[:space:]]*[^#]*i:[[:space:]]*(.*)", REG_EXTENDED);
    regcomp(&r_command, "^[[:space:]]*[^#]*c:[[:space:]]*(.*)", REG_EXTENDED);
    regcomp(&r_title, "^[[:space:]]*[^#]*t:[[:space:]]*(.*)", REG_EXTENDED);

    char buffer[256];
    std::string icon, command, title;
    int element = 0;

    while (file.getline(buffer, sizeof(buffer)))
    {
        if (!regexec(&r_icon, buffer, 2, pmatch, 0))
        {
            icon = std::string(buffer + pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
            element++;
        }
        else if (!regexec(&r_command, buffer, 2, pmatch, 0))
        {
            command = std::string(buffer + pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
            element++;
        }
        else  if (!regexec(&r_title, buffer, 2, pmatch, 0))
        {
            title = std::string(buffer + pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
            element++;
        }

        if (element == 3)
        {
            if (icon != "" && command != "" && title != "")
            {
                App *app = new App(icon, command, title);
                list.push_back(app);
            }

            icon = command = title = "";
            element = 0;
        }
    }

    regfree(&r_icon);
    regfree(&r_command);
    regfree(&r_title);

    file.close();

    return list;
}

void Config::setFile( std::string configFile )
{
   this->configFile = configFile;
}
