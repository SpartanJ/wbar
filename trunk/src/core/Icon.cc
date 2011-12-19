#include "Icon.h"
#include "i18n.h"
#include <string.h>

using namespace std;

Icon::Icon(string iconImg, string cmd, int xx, int yy,
	unsigned long winid, unsigned char *icondata, int iw, int ih) :
    ox(xx), oy(yy), x(xx), y(yy), need_update(1), cx(xx), command(cmd),
    wid(winid){
    if (iconImg == "") {
	icon = imlib_create_image(iw, ih);
	imlib_context_set_image(icon);
	imlib_image_set_has_alpha(1);
    } else if( !(icon = LOAD_IMAGE(iconImg.c_str())) )
	throw (iconImg + _(" Couldn't load icon image.")).c_str();

    USE_IMAGE(icon);
    if (iconImg == "") {
	DATA32 *imagedata = imlib_image_get_data();
	memmove(imagedata,icondata,sizeof(long));
	imlib_image_put_back_data(imagedata);
    }
    osize = size = IMAGE_WIDTH();
}    

Icon::~Icon(){
    FREE_IMAGE(icon);
}

