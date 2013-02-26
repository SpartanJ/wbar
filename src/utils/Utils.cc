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

Utils::Utils()
{
}

Utils::~Utils()
{
}

std::string Utils::replace ( std::string orig, std::string find, std::string replace )
{
    while ( orig.find ( find ) != std::string::npos )
    {
        orig.replace ( orig.find ( find ), find.length(), replace );
    }

    return orig;
}

std::list<std::string> Utils::split ( std::string text, std::string sep )
{
    std::list <std::string> list;

    unsigned long i;
    unsigned long y;

    std::string data, aux;

    while ( !text.empty() && ( i = text.find ( sep ) ) != std::string::npos )
    {
        y = 0;

        data = "";
        aux = "";

        while ( y != i )
        {
            data += text[y++];
        }

        if ( data.length() > 0 )
        {
            list.push_back ( data );
        }

        y++;

        while ( y != text.length() )
        {
            aux += text[y++];
        }

        text = aux;
    }

    if ( text.length() > 0 )
    {
        list.push_back ( text );
    }

    return list;
}

std::string Utils::join ( std::list<std::string> list, std::string sep )
{
    std::string text;
    std::list<std::string>::iterator it;

    for ( it = list.begin(); it != list.end(); it++ )
    {
        text += sep + ( *it );
    }

    return text;
}

