#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <libgen.h>

#include "XWin.h"
#include "ImgWrap.h"
#include "OptParser.h"
#include "SuperBar.h"
#include "Config.h"
#include "Utils.h"
#include "i18n.h"

static size_t configitems;
static unsigned int taskbar;
static std::list<App *> list;
static std::list<App *>::iterator it;
static App *p;

static Bar *barra;
static XWin barwin(50,50,50,50);

unsigned long bg_window;
void corpshandler(int);
int mapIcons();
static int refl_size;
static XErrorHandler oldXHandler = (XErrorHandler) 0 ;
static int eErrorHandler(Display *, XErrorEvent *);

int main(int argc, char **argv)
{
#ifdef ENABLE_NLS
    setlocale( LC_ALL, "" );
    bindtextdomain(GETTEXT_PACKAGE, GNOMELOCALEDIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);
#endif

    /* Variables */
    struct sigaction sigh;

    barra = NULL;
    try
    {
        unsigned int dblclk_tm, butpress, noreload;
        unsigned long dblclk0 = 0;
        int inum, vertbar;

        /* Register handler for recovering corps */
        sigh.sa_handler = corpshandler;
        sigh.sa_flags = 0;
        sigemptyset(&sigh.sa_mask); //exclude all signals
        sigaction(SIGCHLD, &sigh, NULL);

        Config config;

        OptParser tmpoptparser(argc, argv);

        if ( tmpoptparser.isSet( OptParser::CONFIG ) )
        {
           config.setFile( tmpoptparser.getArg( OptParser::CONFIG ) );
        }

        list = config.getAppList();

        if (list.size() != 0)
        {
            it = list.begin();
            p = (*it);
        }
        else
        {
            throw _("Configuration empty.");
        }

        std::string command = p->getCommand();

        if (command.empty())
        {
            command = PACKAGE_NAME" "DEFAULT_ARGV;
        }

        if (argc <= 1 || tmpoptparser.isSet( OptParser::CONFIG ))
        {
            std::list<std::string> list;
            Utils util;
            list = util.split ( command, " " );
            argc = list.size();

            if (argc > 1)
            {
                std::list<std::string>::iterator ac;
                argv = new char * [argc + 1];
                int i = 0;

                for (ac = list.begin();ac != list.end();ac++, i++)
                {
                    argv[i] = strdup((*ac).c_str());
                }

                argv[argc] = NULL;
            }

        }

        XEvent ev;

        OptParser optparser(argc, argv);

        if (optparser.isSet(OptParser::VERS))
        {
            std::cout << _("Version of ") << PACKAGE_NAME << " " << VERSION << std::endl;
            return 0;
        }
        if (optparser.isSet(OptParser::HELP))
        {

            std::cout << _("Usage: wbar [option] ... [option]") << std::endl;
            std::cout << _("Options:") << std::endl;
            std::cout << "   --help         " << _("this help") << std::endl;
            std::cout << "   --version      " << _("show version") << std::endl;
            std::cout << "   --config filepath  " << _("conf-file (eg: $HOME/.wbar)") << std::endl;
            std::cout << "   --above-desk       " << _("run over a desktop app (ie: xfdesktop)") << std::endl;
            std::cout << "   --taskbar		" << _("enable taskbar") << std::endl;
            std::cout << "   --noreload         " << _("right click does not force reload anymore") << std::endl;
            std::cout << "   --offset i         " << _("offset bar (eg: 20)") << std::endl;
            std::cout << "   --isize  i         " << _("icon size (eg: 32)") << std::endl;
            std::cout << "   --idist  d         " << _("icon dist (eg: 1)") << std::endl;
            std::cout << "   --zoomf  z         " << _("zoom factor (eg: 1.8 or 2.5)") << std::endl;
            std::cout << "   --jumpf  j         " << _("jump factor (eg: 1.0 or 0.0)") << std::endl;
            std::cout << "   --pos    p         " << _("position:") << std::endl;
            std::cout << "                        " << "top | bottom | left | right | " << std::endl;
            std::cout << "                        " << "center | <bot|top>-<right|left>" << std::endl;
            std::cout << "   --grow             " << _("investing icons growth") << std::endl;
            std::cout << "   --dblclk ms        " << _("ms for double click (0: single click)") << std::endl;
            std::cout << "   --bpress           " << _("icon gets pressed") << std::endl;
            std::cout << "   --vbar             " << _("vertical bar") << std::endl;
            std::cout << "   --balfa  i         " << _("bar alfa (0-100)") << std::endl;
            std::cout << "   --rsize  i         " << _("Reflection size in percents (0-100)") << std::endl;
            std::cout << "   --falfa  i         " << _("unfocused bar alfa (0-100)") << std::endl;
            std::cout << "   --filter i         " << _("color filter (0: none 1: hovered 2: others, 3: all)") << std::endl;
            std::cout << "   --fc  0xAARRGGBB   " << _("filter color (default green 0xff00c800)") << std::endl;
            std::cout << "   --nanim  i         " << _("number of animated icons: 1, 3, 5, 7, 9, ...") << std::endl;
            std::cout << "   --nofont           " << _("if set disables font rendering") << std::endl;

            std::cout << _("View man(1).") << std::endl;
            return 0;
        }

        /* window configuration */
        if (optparser.isSet(OptParser::ABOVE_DESK))
        {
            barwin.setDockWindow();
            barwin.skipTaskNPager();
            barwin.noDecorations();
            barwin.setSticky();
            barwin.bottomLayer();
        }
        else
        {
            barwin.setOverrideRedirection();
            barwin.lowerWindow();
        }

	barwin.setName(basename(argv[0]));
	
        taskbar = optparser.isSet(OptParser::TASKBAR)?1:0;

        /* tell X what events we're intrested in */
            barwin.selectInput(PointerMotionMask | ExposureMask | 
            ButtonPressMask |ButtonReleaseMask | LeaveWindowMask | 
            EnterWindowMask,taskbar);

        /* Image library set up */
        INIT_IMLIB(barwin.getDisplay(), barwin.getVisual(), barwin.getColormap(),
                barwin.getDrawable(), 2048*2048);

        /* check if double clicking, ms time */
        dblclk_tm = optparser.isSet(OptParser::DBLCLK)?atoi(optparser.getArg(OptParser::DBLCLK).c_str()):0;

        butpress = optparser.isSet(OptParser::BPRESS)?1:0;

        /* check if reload is admited */
        noreload = optparser.isSet(OptParser::NORELOAD)?1:0;

        vertbar = optparser.isSet(OptParser::VBAR)?1:0;
        refl_size = optparser.isSet(OptParser::RSIZE)?atoi(optparser.getArg(OptParser::RSIZE).c_str()):0;

        bool grow = optparser.isSet(OptParser::GROW)?true:false;

        if (optparser.isSet(OptParser::BALFA) || optparser.isSet(OptParser::FALFA) || optparser.isSet(OptParser::FILTER) || !(p->getTitle().empty() || optparser.isSet(OptParser::NOFONT)))
        {
            barra = new SuperBar(&barwin, p->getIconName(), p->getTitle(),
                    optparser.isSet(OptParser::ISIZE)?atoi(optparser.getArg(OptParser::ISIZE).c_str()):32,
                    optparser.isSet(OptParser::IDIST)?atoi(optparser.getArg(OptParser::IDIST).c_str()):1,
                    optparser.isSet(OptParser::ZOOMF)?atof(optparser.getArg(OptParser::ZOOMF).c_str()):1.8,
                    optparser.isSet(OptParser::JUMPF)?atof(optparser.getArg(OptParser::JUMPF).c_str()):1,
                    vertbar,
                    4,
                    optparser.isSet(OptParser::NANIM)?atoi(optparser.getArg(OptParser::NANIM).c_str()):5,
                    optparser.isSet(OptParser::BALFA)?atoi(optparser.getArg(OptParser::BALFA).c_str()):-1,
                    optparser.isSet(OptParser::FALFA)?atoi(optparser.getArg(OptParser::FALFA).c_str()):-1,
                    optparser.isSet(OptParser::FILTER)?atoi(optparser.getArg(OptParser::FILTER).c_str()):0,
                    strtoul((optparser.isSet(OptParser::FC)?optparser.getArg(OptParser::OptParser::FC).c_str():"0xff00c800"), NULL, 16),
                    optparser.isSet(OptParser::NOFONT)?0:1,
                    optparser.isSet(OptParser::OFFSET)?atoi(optparser.getArg(OptParser::OFFSET).c_str()):0,
                    grow);
        }
        else
        {
            barra = new Bar(&barwin, p->getIconName(),
                    optparser.isSet(OptParser::ISIZE)?atoi(optparser.getArg(OptParser::ISIZE).c_str()):32,
                    optparser.isSet(OptParser::IDIST)?atoi(optparser.getArg(OptParser::IDIST).c_str()):1,
                    optparser.isSet(OptParser::ZOOMF)?atof(optparser.getArg(OptParser::ZOOMF).c_str()):1.8,
                    optparser.isSet(OptParser::JUMPF)?atof(optparser.getArg(OptParser::JUMPF).c_str()):1,
                    vertbar,
                    4,
                    optparser.isSet(OptParser::NANIM)?atoi(optparser.getArg(OptParser::NANIM).c_str()):5,
                    optparser.isSet(OptParser::OFFSET)?atoi(optparser.getArg(OptParser::OFFSET).c_str()):0,
                    grow);
        }

        if (p) delete p;
	// note the size of icons in config befor we add any active icons
	configitems = (size_t) list.size();
	//loop until actual window data is obtained
	while (mapIcons());

        /* Show the Bar */
        if (optparser.isSet(OptParser::ABOVE_DESK))
        {
            barwin.mapWindow();
            barra->setPosition(optparser.getArg(OptParser::POS));
        }
        else
        {
            barra->setPosition(optparser.getArg(OptParser::POS));
            barwin.mapWindow();
        }
        barwin.lowerWindow();
        barra->refresh();

        /* Event Loop */
        while (true)
        {
            barwin.nextEvent(&ev);
            switch (ev.type)
            {

            case Expose:
                barra->refresh();
                break;

                /* Button Press */
            case ButtonPress:
            //KDE won't honor stacking order and dock property. Lower explicitly
            barwin.lowerWindow();
                switch (ev.xbutton.button)
                {
                case 1:
                    if (butpress!=0)
                    {
                        if (!vertbar)
                        {
                            if ((inum = barra->iconIndex(ev.xbutton.x))!=-1)
                                barra->iconDown(inum);
                        }
                        else
                        {
                            if ((inum = barra->iconIndex(ev.xbutton.y))!=-1)
                                barra->iconDown(inum);
                        }
                    }
                    break;
                case 4: //wheel up
                    //barra->setZoom(barra->getZoom()+0.1);
                    //barra->scaleIcons(ev.xbutton.x);
                    break;
                case 5:
                    //barra->setZoom(barra->getZoom()-0.1);
                    //barra->scaleIcons(ev.xbutton.x);
                    break;
                }
                break;

                /* Button Release */
            case ButtonRelease:
            // some programs like skype miss their exit notification so the icon
            // still remains in the taskbar. Just reload the bar if such icon clicked
                oldXHandler = XSetErrorHandler(eErrorHandler);
                switch (ev.xbutton.button)
                {
                case 3:/* Redraw Bar*/
                        if(!noreload)
                        {
                            if ( tmpoptparser.isSet( OptParser::CONFIG ) )
                            {
                                execvp(tmpoptparser.getArgv()[0], tmpoptparser.getArgv());
                            }
                            else
                            {
                                execvp(argv[0], argv);
                            }
                        }
                    break;
                case 1:/* Execute Program */
                    if (!vertbar)
                        inum = barra->iconIndex(ev.xbutton.x);
                    else
                        inum = barra->iconIndex(ev.xbutton.y);

                    if (butpress!=0)
                        barra->iconUp(inum);
                    /* Double click time 200 ms */
                    if (barra->iconWinId(inum)) {
                	// raise event may go to a window already gone
                        oldXHandler = XSetErrorHandler(eErrorHandler);
                	barwin.windowAction(barra->iconWinId(inum));
            		(void) XSetErrorHandler(oldXHandler);
            	    } else {
                	if ((ev.xbutton.time - dblclk0 <dblclk_tm || dblclk_tm==0) && inum != -1)
                	{
                    	    if (fork()==0)
                    	    {
                        	if (execlp("sh", "sh", "-c", barra->iconCommand(inum).c_str(), NULL) != 0)
                        	{
                            	    std::cout << _("Error run program: ") << barra->iconCommand(inum) << std::endl;
                        	}
                    	    }

                	}
                	else dblclk0 = ev.xbutton.time;
            	    }
                    break;
                case 2:/* Iconify window */
	            if (!vertbar)
                        inum = barra->iconIndex(ev.xbutton.x);
                    else
                        inum = barra->iconIndex(ev.xbutton.y);
		    if (barra->iconWinId(inum))
			barwin.windowIconify(barra->iconWinId(inum));
		    break;
                }
                (void) XSetErrorHandler(oldXHandler);
                break;

                /* Motion Notify */
            case MotionNotify:
                if (!vertbar)
                    barra->refresh(ev.xmotion.x);
                else
                    barra->refresh(ev.xmotion.y);
                break;

                /* Leave & Enter Notify */
            case LeaveNotify:
                /* NotifyGrab && Ungrab r notified on B1 click*/
                if (ev.xcrossing.mode!=NotifyGrab && !(ev.xcrossing.state&Button1Mask))
                    barra->refresh();
                //hack for kde multiple notifications in grab mode
                barwin.flushAll();
                break;

            case EnterNotify:
                if (ev.xcrossing.mode!=NotifyUngrab && !(ev.xcrossing.state&Button1Mask))
                {
                    if (!vertbar)
                        barra->refresh(ev.xcrossing.x);
                    else
                        barra->refresh(ev.xcrossing.y);
                }
                break;
	    	case PropertyNotify:
			if ((std::string) barwin.atomName(ev.xproperty.atom) 
		    		== "_NET_WM_ICON")
			{
				mapIcons();
		    		if (!barwin.windowFocused())
		    			barra->refreshUnfocused();
				else if (!vertbar)
        		    	    barra->refresh(ev.xcrossing.x);
		    		else
        		    	    barra->refresh(ev.xcrossing.y);
            		}
			if ((std::string) barwin.atomName(ev.xproperty.atom) 
		    		== "_NET_CLIENT_LIST")
			{
				//loop until actual window data is obtained
		    		while (mapIcons());
		    		if (!barwin.windowFocused())
		    			barra->refreshUnfocused();
				    else if (!vertbar)
        		        barra->refresh(ev.xcrossing.x);
		    		else
        		    	barra->refresh(ev.xcrossing.y);
			}
			break;
        	default:
        		break;

            }
        }

    }
    catch (const char *m)
    {
        std::cout << m << std::endl;
    }

    if (barra) delete barra;
    return 0;
}

void corpshandler(int sig)
{
    while (waitpid(-1, NULL, WNOHANG) > 0)
    {

    }
}

int mapIcons()
{
    unsigned char *runningApp;
    unsigned long len, tmp_len, winid;
    std::string pixmapdir = PIXMAPDIR;
    std::string packagename = PACKAGE_NAME;
    std::string icon,cmnd;
    unsigned char *titl;
    int firstrun=0;
    // on the first run, there will be no icons displayed
    if (!barra->iconsShown())
	firstrun=1;
    //roll back the list to contain only the icons from config
    while ((size_t)list.size() > configitems)
    {
	list.pop_back();
	((SuperBar *)barra)->removeIcon();
	it--;
    }
    // add currently running tasks to the list
    if ( (taskbar) && 
	    (runningApp = barwin.windowProp(NULL, "_NET_CLIENT_LIST", &len)) ) {
	long *array = (long*) runningApp;
	// in e16 WM, context menus block XQueue to stay on top and focused 
	// until closed, and so wbar gets and old _NET_CLIENT_LIST with
	// nonexistent windows there, crashing wbar on quering them.
	// Since no windows to display can appear or disappear during that, 
	// we work around the problem by just ignoring BadWindow errors
	oldXHandler = XSetErrorHandler(eErrorHandler);
	for (unsigned long k = 0; k < len; k++){
    	    Window w = (Window) array[k];
    	    if (barwin.issetHint(w, "_NET_WM_WINDOW_TYPE", 
    		    "_NET_WM_WINDOW_TYPE_DESKTOP")&&firstrun) 
    		bg_window = w;
    	    if (!barwin.issetHint(w,"_NET_WM_STATE","_NET_WM_STATE_SKIP_TASKBAR")
    			&& (barwin.issetHint(w,"_NET_WM_WINDOW_TYPE",
			"_NET_WM_WINDOW_TYPE_NORMAL") ||
    			!barwin.haveAtom(w,"_NET_WM_WINDOW_TYPE"))) {
		icon="";
		int iiw,iih;
		if (barwin.windowIcon(w, &iiw, &iih) == NULL)
		    icon=pixmapdir+"/"+packagename+"/"+"questionmark.png";
		cmnd="";
		winid=(unsigned long) w;
		titl=barwin.windowProp(&w,"WM_NAME",&tmp_len);
		// now, THIS is impossible, so we're dealing with an 
		// old NET_CLIENT_LIST
		if (!titl) 
		    continue;
    		const char *title=(const char*) titl;
    		App *app = new App(icon, cmnd, title, winid);
    		list.push_back(app);
    		icon = cmnd = "";
    		titl = NULL;
    		winid=0;
	    }
	}
    }
    //if this is not an initial run
    if (!firstrun)
        // and we added no windows
	if ((size_t)list.size() == configitems)
	    //then there is no need to add icons
	    return 0;
    for (it++; it != list.end(); it++)
    {
        p = (*it);
        try
            {
        	if (p->getIconName() != "")
            	    ((SuperBar *)barra)->addIcon(p->getIconName(), 
            		p->getCommand(), p->getTitle(), p->getWinid(), 
            		NULL, 0, 0, refl_size);
                else {
            	    int iw, ih;
        			barwin.selectWindowInput(p->getWinid()); 
            	    unsigned char *icondata = barwin.windowIcon((Window) p->getWinid(), &iw, &ih);
            	    if (icondata) {
            		((SuperBar *)barra)->addIcon(p->getIconName(), 
            		    p->getCommand(), p->getTitle(), p->getWinid(), 
            		    icondata, iw, ih, refl_size);
            	    } else {
                	std::cout << "window has gone, not adding" << std::endl;
                	return -1;
            	    }
            	}
            }
            catch (const char *m)
            {
                std::cout << m << std::endl;
            }
    	    if (p) delete p;
        }
	(void) XSetErrorHandler(oldXHandler);
	barra->scale();
	if (firstrun)
	    it--;
	return 0;
}

static int eErrorHandler(Display *display, XErrorEvent *theEvent)
{	
	std::cout << "error code:" << (int) theEvent->error_code 
		<< " request code:" << (int) theEvent->request_code << std::endl;
	return 0;
	exit (-1);
    /* No exit! - but keep lint happy */
}

