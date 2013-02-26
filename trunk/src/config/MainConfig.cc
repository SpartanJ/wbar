#include <iostream>
#include <cstdlib>
#include <sys/wait.h>
#include <signal.h>
#include <config.h>
#include <gtk/gtk.h>
#include "Functions.h"
#include "i18n.h"

void signals ( int sig );

int main ( int argc, char ** argv )
{
#ifdef ENABLE_NLS
    setlocale ( LC_ALL, "" );
    bindtextdomain ( GETTEXT_PACKAGE, GNOMELOCALEDIR );
    bind_textdomain_codeset ( GETTEXT_PACKAGE, "UTF-8" );
    textdomain ( GETTEXT_PACKAGE );
#endif

    struct sigaction sigh;

    sigh.sa_handler = signals;
    sigh.sa_flags = 0;
    sigemptyset ( &sigh.sa_mask );
    sigaction ( SIGCHLD, &sigh, NULL );

    gtk_init ( &argc, &argv );

    view_settings();
    gtk_main ();

    return 0;
}

void signals ( int sig )
{
    while ( waitpid ( -1 , NULL, WNOHANG ) > 0 )
    {

    }
}

