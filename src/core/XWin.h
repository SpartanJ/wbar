#ifndef _XWIN_H_
#define _XWIN_H_

/* X11 stuff */
#include <X11/Xlib.h>
#include <string>

class Bar;
class SuperBar;

class XWin
{
    private:
        Display   *  display;
        Visual   *   visual;
        Window      window;
        Window      taskWindow;
        Atom        delWindow;
        Colormap    colormap;
        int     depth;
        int     eventMask;
        int		rootEventMask;

        int x, y;
        int w, h;

        friend class Bar;
        friend class SuperBar;
    public:

        XWin ( int x, int y, int w, int h );
        ~XWin();

        void selectInput ( int mask, int taskbar_set );

        void mapWindow();
        void unMapWindow();
        void lowerWindow();
        void raiseWindow();
        bool nextEvent ( XEvent * ev );
        void setName ( char * name );

        int screenWidth() const;
        int screenHeight() const;
        Display * getDisplay();
        Visual * getVisual();
        Window  getDrawable();
        Colormap getColormap();


        void moveNresize ( int x, int y, int w, int h );

        void setOverrideRedirection ( Bool ovr = True );
        void noDecorations();
        void setDockWindow();
        void setSticky();
        void skipTaskNPager();
        void bottomLayer();
        char * atomName ( Atom atomname );
        unsigned char * windowProp ( Window * targetWin, const char * reqProp, unsigned long * len );
        Atom issetHint ( Window targetWin, const char * reqProp, const char * reqHint );
        Atom * haveAtom ( Window targetWin, const char * reqProp );
        unsigned char * windowIcon ( Window targetWin, int * iw, int * ih );
        void windowAction ( unsigned long winid );
        void windowIconify ( unsigned long winid );
        int windowFocused();
        void selectWindowInput ( unsigned long winid );
};

Pixmap GetRootPixmap ( Display * display );

#endif /* _XWIN_H_ */
