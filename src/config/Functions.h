#ifndef _CALLBACKS_H_
#define _CALLBACKS_H_

#include <gtk/gtk.h>
#include <iostream>

#ifdef __cplusplus
extern "C"
{
#endif

    void view_settings();
    void set_treeview();
    void set_default_icon();
    void set_comboboxs();
    void set_config();
    void set_config_states(std::string command);
    void set_signals();
    void update_treeview();
    GdkPixbuf * get_icon(std::string icon);
    GdkPixbuf * get_icon_size(std::string icon, int width, int heigth);

    void set_status_button();
    void on_settings_new();
    void on_settings_edit();
    void on_settings_delete();
    void on_settings_update();
    void on_settings_close();
    std::string getCommand();
    bool validate();

    void on_app_accept();
    void on_app_cancel();
    void on_app_up();
    void on_app_down();
    void on_treeview_cursor_changed ();
    void on_icon_changed();
    void on_select_command();
    void on_select_icon();
    void on_select_fontname();
    void on_select_imagename();

    gboolean clickTreeView(GtkWidget * widget, GdkEventButton * event, gpointer data);
    void checkbutton_toggled (GtkToggleButton *togglebutton, gpointer data);
    gboolean widget_hide(GtkWidget * widget, GdkEvent * event, gpointer data);
    void show_about();
    
#ifdef __cplusplus
}
#endif

#endif

