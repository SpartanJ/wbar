#ifndef _ICON_H_
#define _ICON_H_

#include <string>
#include "ImgWrap.h"

class Bar;
class SuperBar;

class Icon
{
protected:
    _image icon;

    /* normal state */
    int ox, oy;
    int osize;
    /* current state */
    int x, y;
    int size;

    int bs, bx, by;
    float vx, vs, vy;

    /* redraw ? */
    int need_update;
    /* clean coords */
    int cx, csize;

    std::string command;
    unsigned long wid;

    /* extend friendship? */
    friend class Bar;
    friend class SuperBar;

public:

    Icon(std::string iconImg, std::string cmd, int x, int y,
	unsigned long winid, unsigned char *icondata, int iw, int ih,
	int refl_size);
    ~Icon();
};

#endif /* _ICON_H_ */
