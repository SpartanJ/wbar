#include "Icon.h"
#include "i18n.h"
#include <string.h>


using namespace std;

_image icontmp;

Icon::Icon(string iconImg, string cmd, int xx, int yy,
	unsigned long winid, unsigned char *icondata, int iw, int ih,
	int refl_size) :
    ox(xx), oy(yy), x(xx), y(yy), need_update(1), cx(xx), command(cmd),
    wid(winid){
    
   	int refl_h;
    
    if (iconImg == "") {
		icon = imlib_create_image(iw, ih);
		imlib_context_set_image(icon);
		imlib_image_set_has_alpha(1);
    } else if ( !(icon = LOAD_IMAGE(iconImg.c_str())) )
		throw (iconImg + _(" Couldn't load icon image.")).c_str();

    USE_IMAGE(icon);
    if (iconImg == "") {
		DATA32 *imagedata = imlib_image_get_data();
		memmove(imagedata,icondata,sizeof(long));
		imlib_image_put_back_data(imagedata);
    }
    
    imlib_context_set_blend(1);
    // add reflection to icon
    if (refl_size) {
	    // calculate size for icons loaded from files
    	if (!iw || !ih) {
        	iw=imlib_image_get_width();
        	ih=imlib_image_get_height();
    	}
    	refl_h = ih*(refl_size/100.0);
    	icontmp = imlib_create_image(iw, ih);
    	USE_IMAGE(icontmp);
    	IMAGE_ENABLE_ALPHA(1);
    	imlib_blend_image_onto_image(icon, 1, 0, 0, iw, ih, 0, 0, iw, 
    									ih-refl_h); // was 10
    	imlib_blend_image_onto_image_skewed(icon, 1, 0, 0, iw, ih, 0, ih, iw , 
    										0, 0, -refl_h); // was 10
    	icon=imlib_clone_image();
    }
    USE_IMAGE(icon);
    
    osize = size = IMAGE_WIDTH();
}    

Icon::~Icon(){
    FREE_IMAGE(icon);
}

