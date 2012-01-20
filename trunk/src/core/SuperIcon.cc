#include <string.h>
#include "SuperIcon.h"

using namespace std;

SuperIcon::SuperIcon(string iconImg, string cmd, string txt, 
	unsigned long winid, unsigned char *icondata, int iw, int ih,
	int refl_size,
	int x, int y, int tw, int th) : 
    Icon(iconImg, cmd, x, y, winid, icondata, iw, ih, refl_size), 
	    textW(tw), textH(th), text(txt){
    USE_IMAGE(icon);
    icon_color = CLONE_IMAGE();
}

SuperIcon::~SuperIcon(){
    FREE_IMAGE(icon_color);
}

