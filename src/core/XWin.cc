#include <sys/select.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include "XWin.h"
#include "i18n.h"

using namespace std;

XWin::XWin(int xx, int yy, int ww, int hh) :
    eventMask(NoEventMask), x(xx), y(yy), w(ww), h(hh)
{

    /* if(threads) XInitThreads() */
    int defScreen;
    XSetWindowAttributes attr;
    attr.bit_gravity = StaticGravity;
    attr.backing_store = Always;
    
    if (!(display = XOpenDisplay(NULL)))
        throw _("Couldn't open display");

    defScreen = DefaultScreen(display);
    if (!(visual = DefaultVisual(display, defScreen)))
        throw _("Couldn't get visual");
    if (!(colormap = DefaultColormap(display, defScreen)))
        throw _("Couln't get colormap");
    if (!(depth = DefaultDepth(display, defScreen)))
        throw _("Couldn't get depth");
    if (!(window = XCreateSimpleWindow(display,
            DefaultRootWindow(display), x, y, w, h, 0, 0, 0)))
        throw _("Couldn't create window");

    /* prepare for delete window */
    delWindow = XInternAtom(display, "WM_DELETE_WINDOW", false);
    /* Set WM Protocols to report window delete event */
    XSetWMProtocols(display, window, &delWindow, 1);

    XClassHint ch = {(char *)PACKAGE_NAME, (char *)PACKAGE_NAME};
    XSetClassHint(display, window, &ch);
    XChangeWindowAttributes(display, window, CWBitGravity|CWBackingStore, &attr);
}

XWin::~XWin()
{
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

void XWin::selectInput(int ev_mask)
{
    eventMask = ev_mask|PropertyChangeMask;
    XSelectInput(display, window, ev_mask);
    XSelectInput(display, XDefaultRootWindow(display), PropertyChangeMask);
}

void XWin::lowerWindow()
{
    XLowerWindow(display, window);
}

void XWin::raiseWindow()
{
    XRaiseWindow(display, window);
}

void XWin::mapWindow()
{
    XMapWindow(display, window);
}

void XWin::unMapWindow()
{
    XUnmapWindow(display, window);
}

bool XWin::nextEvent(XEvent *ev)
{


    int qlen;

    do
    {
        /* queue length is relate to my only window */
        if ((qlen = XQLength(display)))
        {
            if (XCheckMaskEvent(display, eventMask, ev) == False)
            {
                /* process events we're not waiting for */
                XSync(display, True);
                continue;
            }
        } else
            XMaskEvent(display, eventMask, ev);
    }
    while ((ev->type == MotionNotify || ev->type == PropertyNotify) && qlen > 1);
    return true;
}

void XWin::setName(char *name)
{
    XTextProperty windowName;
    XStringListToTextProperty(&name, 1, &windowName);

    XSetWMName(display, window, &windowName);
}

void XWin::moveNresize(int x, int y, int w, int h)
{
    XMoveResizeWindow(display, window, x, y, w, h);
}

int XWin::screenWidth() const
{
    return WidthOfScreen(DefaultScreenOfDisplay(display));
}

int XWin::screenHeight()const
{
    return HeightOfScreen(DefaultScreenOfDisplay(display));
}

Display *XWin::getDisplay() { return display; }
Visual *XWin::getVisual() { return visual; }
Colormap XWin::getColormap() { return colormap; }
Drawable XWin::getDrawable() { return window; }

/* Dont allow window manager to decorate / restack / remap / ... */
void XWin::setOverrideRedirection(Bool ovr)
{
    XSetWindowAttributes attr;

    attr.override_redirect = ovr;
    attr.background_pixmap = None; //Copy from back;

    XChangeWindowAttributes(display, window, CWOverrideRedirect | CWBackPixmap  , &attr);
}

void XWin::setDockWindow()
{
    Atom a = XInternAtom(display, "_NET_WM_WINDOW_TYPE", True);

    if (a != None)
    {
        //Atom prop = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DESKTOP", True);
        //Atom prop = XInternAtom(display, "_NET_WM_WINDOW_TYPE_NORMAL", True);
        Atom prop = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DOCK", True);
        XChangeProperty(display, window, a, XA_ATOM, 32, PropModeReplace, (unsigned char *) &prop, 1);
    }
}

void XWin::noDecorations()
{
    Atom a = XInternAtom(display, "_MOTIF_WM_HINTS", True);

    if (a != None)
    {
        long prop[5] = { 2, 0, 0, 0, 0 };
        XChangeProperty(display, window, a, a, 32, PropModeReplace, (unsigned char *) prop, 5);
    }
}

void XWin::setSticky()
{
    Atom a = XInternAtom(display, "_NET_WM_DESKTOP", True);

    if (a != None)
    {
        long prop = 0xFFFFFFFF;
        XChangeProperty(display, window, a, XA_CARDINAL, 32, PropModeAppend, (unsigned char *) &prop, 1);
    }

    a = XInternAtom(display, "_NET_WM_STATE", True);
    if (a != None)
    {
        Atom prop = XInternAtom(display, "_NET_WM_STATE_STICKY", True);
        XChangeProperty(display, window, a, XA_ATOM, 32, PropModeAppend, (unsigned char *) &prop, 1);
    }
}

void XWin::skipTaskNPager()
{

    Atom a = XInternAtom(display, "_NET_WM_STATE", True);
    if (a != None)
    {
        Atom prop = XInternAtom(display, "_NET_WM_STATE_SKIP_TASKBAR", True);
        XChangeProperty(display, window, a, XA_ATOM, 32, PropModeAppend, (unsigned char *) &prop, 1);
    }

    a = XInternAtom(display, "_NET_WM_STATE", True);
    if (a != None)
    {
        Atom prop = XInternAtom(display, "_NET_WM_STATE_SKIP_PAGER", True);
        XChangeProperty(display, window, a, XA_ATOM, 32, PropModeAppend, (unsigned char *) &prop, 1);
    }
}

void XWin::bottomLayer()
{
    Atom a = XInternAtom(display, "_WIN_LAYER", True);
    if (a != None)
    {
        long prop = 0; // 6 is above && _NET_WM_STATE_ABOVE in STATE
        XChangeProperty(display, window, a, XA_CARDINAL, 32, PropModeAppend, (unsigned char *) &prop, 1);
    }

    a = XInternAtom(display, "_NET_WM_STATE", True);
    if (a != None)
    {
        Atom prop = XInternAtom(display, "_NET_WM_STATE_BELOW", True);
        XChangeProperty(display, window, a, XA_ATOM, 32, PropModeAppend, (unsigned char *) &prop, 1);
    }
}

char * XWin::atomName(Atom atomname)
{
    return XGetAtomName(display, atomname);
}

unsigned char * XWin::windowProp(Window *targetWin, const char *reqProp, unsigned long *len){
    Atom a = XInternAtom(display, reqProp, True), actual_type;
    Window tWin;
    Atom proptype;
    const char *workAreaProp = "_NET_WORKAREA";
    const char *viewportProp = "_NET_DESKTOP_VIEWPORT";
    //the following switch would better be organized in some "protos.h" later
    if (targetWin == NULL) {
		tWin = XDefaultRootWindow(display);
	if ((reqProp == workAreaProp) || (reqProp == viewportProp))
		proptype=XA_CARDINAL;
	else
		proptype=XA_WINDOW;
    } else {
		tWin = *targetWin;
		proptype=XA_STRING;
    }

    if (a != None) {
    	int form;
    	unsigned long remain;
    	unsigned char * list;
    	if (XGetWindowProperty(display, tWin, a, 0, 1024, False, proptype, 
			&actual_type, &form, len, &remain, &list) != Success) {
		    	return 0;
		}
    	return list;
    }
    return 0;
}

Atom XWin::issetHint(Window targetWin, const char *reqProp, const char *reqHint){
    Atom a = XInternAtom(display, reqProp, True), actual_type;
    if (a != None) {
	    int form;
    	unsigned long remain;
    	unsigned char * list;
    	unsigned long len;
		//	If not disabled, XErrorHandler will interrupt wbar
    	if (XGetWindowProperty(display, targetWin, a, 0, 1024, False, XA_ATOM, 
			&actual_type, &form, &len, &remain, &list) != Success)
	    		return 0;
    	Atom *array = (Atom*) list;
    	Atom hint=0;
    	for (unsigned int k = 0; k < len; k++) {
		if ((Atom) array[k] == XInternAtom(display, reqHint, True))
	    	    hint=list[k];
	}
	return hint;
    }
    return 0;
}

Atom *XWin::haveAtom(Window targetWin, const char *reqProp){
    Atom a = XInternAtom(display, reqProp, True), actual_type;
    if (a != None) {
	    int form;
    	unsigned long remain;
    	unsigned char * list;
    	unsigned long len;
		//	If not disabled, XErrorHandler will interrupt wbar
    	if (XGetWindowProperty(display, targetWin, a, 0, 1024, False, XA_ATOM, 
			&actual_type, &form, &len, &remain, &list) != Success)
	    		return NULL;
	if (actual_type == None && !form && !remain)
	    return NULL;
	return (Atom*) list;
    }
    return 0;
}

// get icon parameters and data, based on xgeticon by Bertrand Mesot
unsigned char *XWin::windowIcon(Window targetWin, int *iw, int *ih){
    Atom a = XInternAtom(display, "_NET_WM_ICON", True), actual_type;

    if (a != None) {
    int form;
    unsigned long remain;
    unsigned char * list;
    unsigned long len;
//	If not disabled, XErrorHandler will interrupt wbar. Otherwise, remember
//	to handle the returned NULL
    if (XGetWindowProperty(display, targetWin, a, 0, 1, False, XA_CARDINAL, 
	&actual_type, &form, &len, &remain, &list) != Success) {
	    return 0;
	}
    if (!list)
	return NULL;
    *iw = *(int*)list;
    if (XGetWindowProperty(display, targetWin, a, 1, 1, False, XA_CARDINAL, 
	&actual_type, &form, &len, &remain, &list) != Success) {
	    throw _("windowIcon() -- XGetWindowProperty");
	    return 0;
	}
    *ih = *(int*)list;
//	If not disabled, XErrorHandler will interrupt wbar.
    if (XGetWindowProperty(display, targetWin, a, 10, (*iw)*(*ih), False, XA_CARDINAL, 
		&actual_type, &form, &len, &remain, &list) != Success) {
		    return 0;
	}
    return list;
    }
    return 0;
}

void XWin::windowAction(unsigned long winid){
	Window window_return;
	Atom a;
    XClientMessageEvent xev;
	int coordx, coordy;
	unsigned char *virtroot, *desknum;
	unsigned long ltemp;
	short toLeft=0;
	short toUpper=0;
	virtroot=XWin::windowProp(NULL,"_NET_VIRTUAL_ROOTS",&ltemp);
	if ((XWin::windowProp(NULL,"_NET_NUMBER_OF_DESKTOPS",&ltemp)) &&
			(desknum=XWin::windowProp((Window*) (&winid),
			"_NET_WM_DESKTOP",&ltemp))) {
	    a = XInternAtom(display, "_CURRENT_DESKTOP", True);
	    xev.type = ClientMessage;
	    xev.format = 32;
	    xev.data.l[0] = (long) *desknum;
	    xev.data.l[1] = CurrentTime;
	    XSendEvent(display, XDefaultRootWindow(display), False,
        	(SubstructureNotifyMask | SubstructureRedirectMask),
        	(XEvent *)&xev);
        }
	if (virtroot) {
	    XTranslateCoordinates (display, winid, (Window) *virtroot,
    				0, 0, & coordx, & coordy, & window_return);
	    long *virtDesk=(long*) XWin::windowProp(NULL,"_NET_WORKAREA",&ltemp);
	    long *useDesk=(long*) XWin::windowProp(NULL,"_NET_DESKTOP_VIEWPORT",&ltemp);
	    if (coordx < 0)
		toLeft = -1;
	    if (coordy < 0)
		toUpper = -1;
	    useDesk[0]+=coordx/virtDesk[2]*virtDesk[2]+toLeft*virtDesk[2];
	    useDesk[1]+=coordy/virtDesk[3]*virtDesk[3]+toUpper*virtDesk[3];
   	    a = XInternAtom(display, "_NET_DESKTOP_VIEWPORT", True);
	    xev.type = ClientMessage;
	    xev.format = 32;
   	    xev.data.l[0] = useDesk[0];
   	    xev.data.l[1] = useDesk[1];
   	    xev.window = XDefaultRootWindow(display);
   	    xev.message_type = a;
   	    XSendEvent(display, XDefaultRootWindow(display), False,
        	(SubstructureNotifyMask | SubstructureRedirectMask),
        	(XEvent *)&xev);
        }
	a = XInternAtom(display, "_NET_ACTIVE_WINDOW", True);
   	xev.message_type = a;
    xev.window = (Window) winid;
   	xev.data.l[0] = 0;
    xev.data.l[1] = CurrentTime;
   	xev.data.l[2] = 0;
   	XSendEvent(display, XDefaultRootWindow(display), False,
          (SubstructureNotifyMask | SubstructureRedirectMask),
          (XEvent *)&xev);
    //KWin needs explicit raise event
    XRaiseWindow (display,(Window) winid);
}

void XWin::windowIconify(unsigned long winid){
	if (!XWin::issetHint((Window) winid,"_NET_WM_STATE","__NET_WM_STATE_HIDDEN")) 
		XIconifyWindow(display, (Window) winid, DefaultScreen(display));
}

int XWin::windowFocused(){
	int revert;
	Window win_return;
	XGetInputFocus(display,&win_return,&revert);
	if (win_return == window)
		return 1;
	return 0;
}

void XWin::selectWindowInput(unsigned long winid)
{
//    eventMask = ev_mask|PropertyChangeMask;
    XSelectInput(display, (Window) winid, PropertyChangeMask);
}

void XWin::flushAll()
{
    XSync(display,True);
}

