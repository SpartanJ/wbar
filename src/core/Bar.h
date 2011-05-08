#ifndef _BAR_H_
#define _BAR_H_

#include <vector>
#include <string>
#include "ImgWrap.h"
#include "XWin.h"
#include "Icon.h"

class Bar
{
protected:
    /* Drawing buffs */
    _image buffer;
    _image cleaning_buffer;
    _image barback;
    _image bar;

    /* reference to the container window */
    XWin *window;

    std::vector<Icon *> icons;

    /* offset bar */
    int bOffset;
    /* dist. between */
    int icon_dist;
    /* icon size */
    int icon_size;
    /* how many icons move */
    int icon_anim;
    /* jump factor on hover */
    float jump_factor;
    /* zoom factor on hover */
    float zoom_factor;

    /* fast math constants */
    float b_scl_d;
    float b_scl_a;
    float b_scl_b;
    float b_pos_m;
    float b_pos_n;
    float b_dd;
    float icon_offset;
    int icon_unit;
    /* animated icons on one side */
    int icon_ansd;

    /* bar position in window */
    int x, y;
    /* original bar dimensions */
    int owidth, oheight;
    /* actual dimensions */
    int width, height;
    int orientation;
    int position;
    int grow;
    /* actual zoomed icon */
    int zoomed_icon;
    /* is bar focused? */
    int focused;

    /* set bar */
    void focus();
    virtual void unfocus();
#ifndef NO_EXPAND
    void expand(bool inverse);
#endif

    void iconPress(int i_num, int offs);

    void transform(int mousex);
    virtual void render();

    void acquireBack();
    void drawBack();
    virtual void cleanBack();

    void scale(bool updateBG = true);

public:

    Bar(XWin *win, std::string barImg, int iSize, int iDist, float zFactor,
        float jFactor, int bOrient, int bPosition, int nAnim, int offset, int grow);

    virtual ~Bar();

    /* add an Icon */
    void addIcon(std::string path, std::string comm);

    /* Icon information */
    int iconIndex(int mouse_x);
    std::string iconCommand(int i_num);

    /* Focus & unfocus events */
    void refresh(int mouse_x = -1);

    /* Icon press events */
    void iconDown(int i_num);
    void iconUp(int i_num);
    void setPosition(std::string p);
    void setGrow(std::string g);

    //void setZoom(float zoomf);
    //float getZoom();
};

#endif /* _BAR_H_ */
