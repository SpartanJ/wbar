#include "Functions.h"
#include <glade/glade.h>
#include <gdk/gdk.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string.h>
#include "i18n.h"
#include "Utils.h"
#include "Config.h"
#include "Run.h"
#include "OptParser.h"

#define ICON_DEFAULT PIXMAPDIR"/"PACKAGE_NAME".png"

static const gchar *authors[] =
{
    "Yadickson Soto <yadickson@gmail.com>",
    "Rodolfo Granata <warlock.cc@gmail.com>",
    "Ivan Fitenko <ivan.fitenko@eltrino.com>",
    NULL
};

static const gchar *documenters[] =
{
    "Yadickson Soto <yadickson@gmail.com>",
    NULL
};

static const gchar * translater = "Español - Yadickson Soto <yadickson@gmail.com>\nDeutsch - Daniel MeissWilhelm <meisssw01@googlemail.com>";

static GladeXML * xml = NULL;
static bool edit = false;
std::string configFile;

enum GrowIcons
{
    GROW_NORMAL,
    GROW_INVERTED
};

enum Positions
{
    POS_TOP,
    POS_BOTTOM,
    POS_LEFT,
    POS_RIGHT,
    POS_CENTER,
    POS_BOTTOM_LEFT,
    POS_BOTTOM_RIGHT,
    POS_TOP_LEFT,
    POS_TOP_RIGHT
};

enum Filters
{
    FILTER_NONE,
    FILTER_HOVERED,
    FILTER_OTHERS,
    FILTER_ALL
};

enum 
{
    TREE_COLUMN_ICON,
    TREE_COLUMN_TITLE,
    TREE_COLUMN_COMMAND,
    TREE_COLUMN_ICON_NAME,
    TREE_COLUMNS
};

enum
{
    COMBO_COLUMN_TEXT,
    COMBO_COLUMN_ID,
    COMBO_COLUMNS
};

enum
{
    ICON_COLUMN_PIXBUF,
    ICON_COLUMN_TEXT,
    ICON_COLUMN_ID,
    ICON_COLUMNS
};

void view_settings()
{
    glade_init();
    xml = glade_xml_new (GLADE_PACKAGE_NAME, NULL, NULL);
 
    set_default_icon();

    GtkWidget * settings = glade_xml_get_widget (xml, "dialog_settings");
    gtk_widget_show (GTK_WIDGET (settings));
    gtk_widget_show_all (settings);

    set_signals();
    set_treeview();
    set_comboboxs();
    set_config();
    update_treeview();
}

void set_default_icon()
{
    GdkPixbuf * pixbuf;
    pixbuf = gdk_pixbuf_new_from_file (ICON_DEFAULT, NULL);
    gtk_window_set_default_icon(pixbuf);
    g_object_unref (pixbuf);
}

void set_treeview()
{
    GtkWidget * treeview = glade_xml_get_widget (xml, "treeview_app");
    GtkListStore *list_store = gtk_list_store_new (TREE_COLUMNS, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (list_store));

    GtkTreeViewColumn *column;
    GtkCellRenderer *renderer;

    renderer = gtk_cell_renderer_pixbuf_new ();
    column = gtk_tree_view_column_new_with_attributes (_("Icon"), renderer, "pixbuf", TREE_COLUMN_ICON, NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes (_("Title"), renderer, "markup", TREE_COLUMN_TITLE, NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes (_("Command"), renderer, "markup", TREE_COLUMN_COMMAND, NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
}

void set_comboboxs()
{
    GtkWidget * combo;
    GtkListStore *list_store;
    GtkTreeIter iter;
    GtkCellRenderer *cell; 

    combo = glade_xml_get_widget (xml, "combobox_pos");
    list_store = gtk_list_store_new (COMBO_COLUMNS, G_TYPE_STRING, G_TYPE_INT);
    gtk_combo_box_set_model (GTK_COMBO_BOX(combo), GTK_TREE_MODEL (list_store));

    cell = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), cell, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo), cell, "text", 0, NULL); 

    gtk_list_store_append(list_store, &iter);
    gtk_list_store_set (list_store, &iter, COMBO_COLUMN_TEXT, _("top"), COMBO_COLUMN_ID, POS_TOP, -1);

    gtk_list_store_append (list_store, &iter);
    gtk_list_store_set (list_store, &iter, COMBO_COLUMN_TEXT, _("bottom"), COMBO_COLUMN_ID, POS_BOTTOM, -1);

    gtk_list_store_append (list_store, &iter);
    gtk_list_store_set (list_store, &iter, COMBO_COLUMN_TEXT, _("left"), COMBO_COLUMN_ID, POS_LEFT, -1);

    gtk_list_store_append (list_store, &iter);
    gtk_list_store_set (list_store, &iter, COMBO_COLUMN_TEXT, _("right"), COMBO_COLUMN_ID, POS_RIGHT, -1);

    gtk_list_store_append (list_store, &iter);
    gtk_list_store_set (list_store, &iter, COMBO_COLUMN_TEXT, _("center"), COMBO_COLUMN_ID, POS_CENTER, -1);

    gtk_list_store_append (list_store, &iter);
    gtk_list_store_set (list_store, &iter, COMBO_COLUMN_TEXT, _("bot-left"), COMBO_COLUMN_ID, POS_BOTTOM_LEFT, -1);

    gtk_list_store_append (list_store, &iter);
    gtk_list_store_set (list_store, &iter, COMBO_COLUMN_TEXT, _("bot-right"), COMBO_COLUMN_ID, POS_BOTTOM_RIGHT, -1);

    gtk_list_store_append (list_store, &iter);
    gtk_list_store_set (list_store, &iter, COMBO_COLUMN_TEXT, _("top-left"), COMBO_COLUMN_ID, POS_TOP_LEFT, -1);

    gtk_list_store_append (list_store, &iter);
    gtk_list_store_set (list_store, &iter, COMBO_COLUMN_TEXT, _("top-right"), COMBO_COLUMN_ID, POS_TOP_RIGHT, -1);

    gtk_combo_box_set_active (GTK_COMBO_BOX (combo), POS_RIGHT);

    combo = glade_xml_get_widget (xml, "combobox_filter");
    list_store = gtk_list_store_new (COMBO_COLUMNS, G_TYPE_STRING, G_TYPE_INT);
    gtk_combo_box_set_model (GTK_COMBO_BOX(combo), GTK_TREE_MODEL (list_store));

    cell = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), cell, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo), cell, "text", 0, NULL); 

    gtk_list_store_append(list_store, &iter);
    gtk_list_store_set (list_store, &iter, COMBO_COLUMN_TEXT, _("none"), COMBO_COLUMN_ID, FILTER_NONE, -1);

    gtk_list_store_append(list_store, &iter);
    gtk_list_store_set (list_store, &iter, COMBO_COLUMN_TEXT, _("hovered"), COMBO_COLUMN_ID, FILTER_HOVERED, -1);

    gtk_list_store_append(list_store, &iter);
    gtk_list_store_set (list_store, &iter, COMBO_COLUMN_TEXT, _("others"), COMBO_COLUMN_ID, FILTER_OTHERS, -1);

    gtk_list_store_append(list_store, &iter);
    gtk_list_store_set (list_store, &iter, COMBO_COLUMN_TEXT, _("all"), COMBO_COLUMN_ID, FILTER_ALL, -1);

    gtk_combo_box_set_active (GTK_COMBO_BOX (combo), FILTER_NONE);

}

void set_config()
{
    Config config;    
    std::list <App *> list = config.getAppList();

    if (list.size() == 0) return;

    std::list <App *>::iterator it;
    it = list.begin();

    App * app = (*it);

    if (app)
    {
        Utils util;
        std::string font = app->getTitle();
        int size = 10;
        std::list <std::string> f = util.split(font, "/");

        try
        {
            size = (f.size() > 1 ? (int)strtold(f.back().c_str(), NULL) : 10);
            f.pop_back();
        }
        catch(...)
        {
        }

        font = util.join(f, "/");

        GtkWidget * spinbutton_font_size = glade_xml_get_widget (xml, "spinbutton_font_size");
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (spinbutton_font_size), size);

        GtkWidget * entry_font_name = glade_xml_get_widget (xml, "entry_font_name");
        gtk_entry_set_text (GTK_ENTRY (entry_font_name), font.c_str());

        GtkWidget * entry_image_name = glade_xml_get_widget (xml, "entry_image_name");
        gtk_entry_set_text (GTK_ENTRY (entry_image_name), app->getIconName().c_str());

        set_config_states(app->getCommand());
        delete app;
    }

}

void set_config_states(std::string command)
{
    GtkWidget * checkbutton = NULL;
    GtkWidget * spinbutton = NULL;

    Run run;
    int argc;
    char **argv = run.getArg(argc);

    if (argc <= 1)
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

    OptParser tmpoptparser(argc, argv);

    if ( tmpoptparser.isSet( OptParser::CONFIG ) )
    {
       configFile = tmpoptparser.getArg( OptParser::CONFIG );
       Config config;
       config.setFile (configFile);
       config.getAppList ();

        std::list<App *> list = config.getAppList();
        std::list<App *>::iterator it;
        App *p;

        if (list.size() != 0)
        {
            it = list.begin();
            p = (*it);
        }
        else
        {
            throw _("Configuration empty.");
        }

        command = p->getCommand();
        delete p;
        
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
                if (argc > 0) delete[] argv;
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
    }

    OptParser opt(argc, argv);

    if(opt.isSet(OptParser::BPRESS))
    {
        checkbutton = glade_xml_get_widget (xml, "checkbutton_bpress");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton), true);
    }
    if(opt.isSet(OptParser::ABOVE_DESK))
    {
        checkbutton = glade_xml_get_widget (xml, "checkbutton_above_desk");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton), true);
    }
    if(opt.isSet(OptParser::VBAR))
    {
        checkbutton = glade_xml_get_widget (xml, "checkbutton_vbar");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton), true);
    }
    if(opt.isSet(OptParser::NOFONT))
    {
        checkbutton = glade_xml_get_widget (xml, "checkbutton_nofont");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton), true);
    }
    if(opt.isSet(OptParser::POS))
    {
        checkbutton = glade_xml_get_widget (xml, "checkbutton_pos");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton), true);
        GtkWidget * combo = glade_xml_get_widget (xml, "combobox_pos");

        std::string pos = opt.getArg(OptParser::POS);

        if (pos == "top") gtk_combo_box_set_active (GTK_COMBO_BOX (combo), POS_TOP);
        else if (pos == "bottom") gtk_combo_box_set_active (GTK_COMBO_BOX (combo), POS_BOTTOM);
        else if (pos == "left") gtk_combo_box_set_active (GTK_COMBO_BOX (combo), POS_LEFT);
        else if (pos == "center") gtk_combo_box_set_active (GTK_COMBO_BOX (combo), POS_CENTER);
        else if (pos == "bot-left") gtk_combo_box_set_active (GTK_COMBO_BOX (combo), POS_BOTTOM_LEFT);
        else if (pos == "bot-right") gtk_combo_box_set_active (GTK_COMBO_BOX (combo), POS_BOTTOM_RIGHT);
        else if (pos == "top-left") gtk_combo_box_set_active (GTK_COMBO_BOX (combo), POS_TOP_LEFT);
        else if (pos == "top-right") gtk_combo_box_set_active (GTK_COMBO_BOX (combo), POS_TOP_RIGHT);
        else /*if (pos == "right")*/ gtk_combo_box_set_active (GTK_COMBO_BOX (combo), POS_RIGHT);
    }
    if(opt.isSet(OptParser::GROW))
    {
        checkbutton = glade_xml_get_widget (xml, "checkbutton_grow");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton), true);
    }
    if(opt.isSet(OptParser::OFFSET))
    {
        checkbutton = glade_xml_get_widget (xml, "checkbutton_offset");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton), true);
        spinbutton = glade_xml_get_widget (xml, "spinbutton_offset");
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (spinbutton), atoi(opt.getArg(OptParser::OFFSET).c_str()));
    }
    if(opt.isSet(OptParser::NORELOAD))
    {
        checkbutton = glade_xml_get_widget (xml, "checkbutton_noreload");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton), true);
    }
    if(opt.isSet(OptParser::TASKBAR))
    {
        checkbutton = glade_xml_get_widget (xml, "checkbutton_taskbar");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton), true);
    }
    if(opt.isSet(OptParser::RSIZE))
    {
        checkbutton = glade_xml_get_widget (xml, "checkbutton_rsize");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton), true);
        spinbutton = glade_xml_get_widget (xml, "spinbutton_rsize");
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (spinbutton), atoi(opt.getArg(OptParser::RSIZE).c_str()));
    }
    if(opt.isSet(OptParser::ISIZE))
    {
        checkbutton = glade_xml_get_widget (xml, "checkbutton_isize");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton), true);
        spinbutton = glade_xml_get_widget (xml, "spinbutton_isize");
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (spinbutton), atoi(opt.getArg(OptParser::ISIZE).c_str()));
    }
    if(opt.isSet(OptParser::IDIST))
    {
        checkbutton = glade_xml_get_widget (xml, "checkbutton_idist");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton), true);
        spinbutton = glade_xml_get_widget (xml, "spinbutton_idist");
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (spinbutton), atoi(opt.getArg(OptParser::IDIST).c_str()));
    }
    if(opt.isSet(OptParser::NANIM))
    {
        checkbutton = glade_xml_get_widget (xml, "checkbutton_nanim");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton), true);
        spinbutton = glade_xml_get_widget (xml, "spinbutton_nanim");
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (spinbutton), atoi(opt.getArg(OptParser::NANIM).c_str()));
    }
    if(opt.isSet(OptParser::ZOOMF))
    {
        checkbutton = glade_xml_get_widget (xml, "checkbutton_zoomf");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton), true);
        spinbutton = glade_xml_get_widget (xml, "spinbutton_zoomf");
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (spinbutton), atof(opt.getArg(OptParser::ZOOMF).c_str()));
    }
    if(opt.isSet(OptParser::JUMPF))
    {
        checkbutton = glade_xml_get_widget (xml, "checkbutton_jumpf");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton), true);
        spinbutton = glade_xml_get_widget (xml, "spinbutton_jumpf");
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (spinbutton), atof(opt.getArg(OptParser::JUMPF).c_str()));
    }
    if(opt.isSet(OptParser::DBLCLK))
    {
        checkbutton = glade_xml_get_widget (xml, "checkbutton_dblclk");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton), true);
        spinbutton = glade_xml_get_widget (xml, "spinbutton_dblclk");
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (spinbutton), atoi(opt.getArg(OptParser::DBLCLK).c_str()));
    }
    if(opt.isSet(OptParser::BALFA))
    {
        checkbutton = glade_xml_get_widget (xml, "checkbutton_balfa");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton), true);
        spinbutton = glade_xml_get_widget (xml, "spinbutton_balfa");
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (spinbutton), atoi(opt.getArg(OptParser::BALFA).c_str()));
    }
    if(opt.isSet(OptParser::FALFA))
    {
        checkbutton = glade_xml_get_widget (xml, "checkbutton_falfa");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton), true);
        spinbutton = glade_xml_get_widget (xml, "spinbutton_falfa");
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (spinbutton), atoi(opt.getArg(OptParser::FALFA).c_str()));
    }
    if(opt.isSet(OptParser::FILTER))
    {
        checkbutton = glade_xml_get_widget (xml, "checkbutton_filter");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton), true);
        GtkWidget * combo = glade_xml_get_widget (xml, "combobox_filter");

        std::string filter = opt.getArg(OptParser::FILTER);
        int f;
        std::stringstream ss(filter);
        ss >> f;
        
        if (f == 1) gtk_combo_box_set_active (GTK_COMBO_BOX (combo), FILTER_HOVERED);
        else if (f == 2) gtk_combo_box_set_active (GTK_COMBO_BOX (combo), FILTER_OTHERS);
        else if (f == 3) gtk_combo_box_set_active (GTK_COMBO_BOX (combo), FILTER_ALL);
        else /*if (filter == "none")*/ gtk_combo_box_set_active (GTK_COMBO_BOX (combo), FILTER_NONE);
    }
    
    if(opt.isSet(OptParser::FC))
    {
        checkbutton = glade_xml_get_widget (xml, "checkbutton_fc");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton), true);

        std::string scolor = opt.getArg(OptParser::FC);
        std::string salpha = scolor.substr (2, 2);
        scolor = "#" + scolor.substr (4, 6);

        GdkColor rgb;
        GtkWidget * color = glade_xml_get_widget (xml, "colorbutton_fc");

        gdk_color_parse (scolor.c_str(), &rgb);
        gtk_color_button_set_color (GTK_COLOR_BUTTON (color), &rgb);
        gtk_color_button_set_use_alpha (GTK_COLOR_BUTTON (color), TRUE);
        gtk_color_button_set_alpha (GTK_COLOR_BUTTON (color), 256*(strtol ( salpha.c_str(), NULL, 16 ) + 0.5));
    }

    if (argc > 0) delete[] argv;
}

void set_signals()
{

    glade_xml_signal_connect (xml, "on_dialog_settings_destroy", (GCallback) on_settings_close);

    glade_xml_signal_connect (xml, "on_menu_item_new_activate", (GCallback) on_settings_new);
    glade_xml_signal_connect (xml, "on_menu_item_edit_activate", (GCallback) on_settings_edit);
    glade_xml_signal_connect (xml, "on_menu_item_delete_activate", (GCallback) on_settings_delete);

    glade_xml_signal_connect (xml, "on_button_new_clicked", (GCallback) on_settings_new);
    glade_xml_signal_connect (xml, "on_button_edit_clicked", (GCallback) on_settings_edit);
    glade_xml_signal_connect (xml, "on_button_delete_clicked", (GCallback) on_settings_delete);

    glade_xml_signal_connect (xml, "on_button_up_activate", (GCallback) on_app_up);
    glade_xml_signal_connect (xml, "on_button_up_released", (GCallback) on_app_up);

    glade_xml_signal_connect (xml, "on_button_down_activate", (GCallback) on_app_down);
    glade_xml_signal_connect (xml, "on_button_down_released", (GCallback) on_app_down);

    glade_xml_signal_connect (xml, "on_button_app_accept_activate", (GCallback) on_app_accept);
    glade_xml_signal_connect (xml, "on_button_app_accept_released", (GCallback) on_app_accept);

    glade_xml_signal_connect (xml, "on_button_app_cancel_activate", (GCallback) on_app_cancel);
    glade_xml_signal_connect (xml, "on_button_app_cancel_released", (GCallback) on_app_cancel);

    glade_xml_signal_connect (xml, "on_button_update_activate", (GCallback) on_settings_update);
    glade_xml_signal_connect (xml, "on_button_update_released", (GCallback) on_settings_update);

    glade_xml_signal_connect (xml, "on_button_close_activate", (GCallback) on_settings_close);
    glade_xml_signal_connect (xml, "on_button_close_released", (GCallback) on_settings_close);

    glade_xml_signal_connect (xml, "on_menu_item_edit_activate", (GCallback) on_settings_edit);
    glade_xml_signal_connect (xml, "on_menu_item_delete_activate", (GCallback) on_settings_delete);

    glade_xml_signal_connect (xml, "on_button_command_activate", (GCallback) on_select_command);
    glade_xml_signal_connect (xml, "on_button_command_released", (GCallback) on_select_command);

    glade_xml_signal_connect (xml, "on_button_icon_activate", (GCallback) on_select_icon);
    glade_xml_signal_connect (xml, "on_button_icon_released", (GCallback) on_select_icon);

    glade_xml_signal_connect (xml, "on_button_image_name_activate", (GCallback) on_select_imagename);
    glade_xml_signal_connect (xml, "on_button_image_name_released", (GCallback) on_select_imagename);

    glade_xml_signal_connect (xml, "on_button_font_name_activate", (GCallback) on_select_fontname);
    glade_xml_signal_connect (xml, "on_button_font_name_released", (GCallback) on_select_fontname);

    glade_xml_signal_connect (xml, "on_treeview_app_cursor_changed", (GCallback) on_treeview_cursor_changed);
    glade_xml_signal_connect (xml, "on_combobox_icon_changed", (GCallback) on_icon_changed);

    glade_xml_signal_connect (xml, "on_button_about_activate", (GCallback) show_about);
    glade_xml_signal_connect (xml, "on_button_about_released", (GCallback) show_about);

    GtkWidget * check;
    GtkWidget * widget;

    check = glade_xml_get_widget (xml, "checkbutton_pos");
    widget = glade_xml_get_widget (xml, "combobox_pos");
    g_signal_connect (G_OBJECT (check), "toggled", G_CALLBACK (checkbutton_toggled), widget);

    check = glade_xml_get_widget (xml, "checkbutton_offset");
    widget = glade_xml_get_widget (xml, "spinbutton_offset");
    g_signal_connect (G_OBJECT (check), "toggled", G_CALLBACK (checkbutton_toggled), widget);

    check = glade_xml_get_widget (xml, "checkbutton_rsize");
    widget = glade_xml_get_widget (xml, "spinbutton_rsize");
    g_signal_connect (G_OBJECT (check), "toggled", G_CALLBACK (checkbutton_toggled), widget);

    check = glade_xml_get_widget (xml, "checkbutton_isize");
    widget = glade_xml_get_widget (xml, "spinbutton_isize");
    g_signal_connect (G_OBJECT (check), "toggled", G_CALLBACK (checkbutton_toggled), widget);

    check = glade_xml_get_widget (xml, "checkbutton_idist");
    widget = glade_xml_get_widget (xml, "spinbutton_idist");
    g_signal_connect (G_OBJECT (check), "toggled", G_CALLBACK (checkbutton_toggled), widget);

    check = glade_xml_get_widget (xml, "checkbutton_nanim");
    widget = glade_xml_get_widget (xml, "spinbutton_nanim");
    g_signal_connect (G_OBJECT (check), "toggled", G_CALLBACK (checkbutton_toggled), widget);

    check = glade_xml_get_widget (xml, "checkbutton_zoomf");
    widget = glade_xml_get_widget (xml, "spinbutton_zoomf");
    g_signal_connect (G_OBJECT (check), "toggled", G_CALLBACK (checkbutton_toggled), widget);

    check = glade_xml_get_widget (xml, "checkbutton_jumpf");
    widget = glade_xml_get_widget (xml, "spinbutton_jumpf");
    g_signal_connect (G_OBJECT (check), "toggled", G_CALLBACK (checkbutton_toggled), widget);

    check = glade_xml_get_widget (xml, "checkbutton_dblclk");
    widget = glade_xml_get_widget (xml, "spinbutton_dblclk");
    g_signal_connect (G_OBJECT (check), "toggled", G_CALLBACK (checkbutton_toggled), widget);

    check = glade_xml_get_widget (xml, "checkbutton_balfa");
    widget = glade_xml_get_widget (xml, "spinbutton_balfa");
    g_signal_connect (G_OBJECT (check), "toggled", G_CALLBACK (checkbutton_toggled), widget);

    check = glade_xml_get_widget (xml, "checkbutton_falfa");
    widget = glade_xml_get_widget (xml, "spinbutton_falfa");
    g_signal_connect (G_OBJECT (check), "toggled", G_CALLBACK (checkbutton_toggled), widget);

    check = glade_xml_get_widget (xml, "checkbutton_filter");
    widget = glade_xml_get_widget (xml, "combobox_filter");
    g_signal_connect (G_OBJECT (check), "toggled", G_CALLBACK (checkbutton_toggled), widget);

    check = glade_xml_get_widget (xml, "checkbutton_fc");
    widget = glade_xml_get_widget (xml, "colorbutton_fc");
    g_signal_connect (G_OBJECT (check), "toggled", G_CALLBACK (checkbutton_toggled), widget);

    widget = glade_xml_get_widget (xml, "treeview_app");
    GtkWidget * menu = glade_xml_get_widget (xml, "menu_popup");
    g_signal_connect (G_OBJECT (widget), "button-release-event", G_CALLBACK (clickTreeView), menu);
}

void update_treeview()
{
    GtkWidget * treeview = glade_xml_get_widget (xml, "treeview_app");
    GtkListStore *list_store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (treeview)));

    gtk_list_store_clear(list_store);

    GtkTreeIter iter;
    GdkPixbuf *pix;

    Config config;
    std::list <App *> list = config.getAppList();
    std::list <App *>::iterator it;

    for (it = list.begin(), it++; it != list.end() ; it++)
    {
        App * app = (*it);

        if (app)
        {
            pix = get_icon(app->getIconName());
            gtk_list_store_append (list_store, &iter);
            gtk_list_store_set (list_store, &iter, \
                                TREE_COLUMN_ICON, pix, \
                                TREE_COLUMN_TITLE, app->getTitle().c_str(), \
                                TREE_COLUMN_COMMAND, app->getCommand().c_str(), \
                                TREE_COLUMN_ICON_NAME, app->getIconName().c_str(), -1);
            g_object_unref (pix);
            delete app;
        }
    }

    list.clear();
}

GdkPixbuf * get_icon(std::string icon)
{
    return get_icon_size(icon, 32, 32);
}

GdkPixbuf * get_icon_size(std::string icon, int width, int heigth)
{
    if (icon.length() == 0)
    {
        return NULL;
    }

    GdkPixbuf *pixbuf = NULL;

    try
    {
        GError *error = NULL;
        pixbuf = gdk_pixbuf_new_from_file (icon.c_str(), &error);

        if (!pixbuf)
        {
            g_error_free (error);
            return NULL;
        }

        GdkPixbuf *pixbuf_scaled = NULL;
        pixbuf_scaled = gdk_pixbuf_scale_simple (pixbuf, width, heigth, GDK_INTERP_BILINEAR);
        g_object_unref (pixbuf);

        return pixbuf_scaled;
    }
    catch(...)
    {
    }

    return NULL;
}

void on_settings_new()
{
    edit = false;

    GtkWidget * title = glade_xml_get_widget (xml, "entry_title");
    GtkWidget * command = glade_xml_get_widget (xml, "entry_command");
    GtkWidget * icon = glade_xml_get_widget (xml, "entry_icon");

    gtk_entry_set_text (GTK_ENTRY(title), "");
    gtk_entry_set_text (GTK_ENTRY(command), "");
    gtk_entry_set_text (GTK_ENTRY(icon), "");

    GtkWidget * dialog = glade_xml_get_widget (xml, "dialog_app");
    gtk_window_set_title (GTK_WINDOW (dialog), _("New"));
    g_signal_connect (G_OBJECT (dialog), "delete-event", G_CALLBACK (widget_hide), NULL);
    gtk_widget_show (dialog);
}

void on_settings_edit()
{
    edit = true;
    GtkWidget * treeview = glade_xml_get_widget (xml, "treeview_app");
 
    GtkTreeSelection *selection;
    GtkTreeModel     *model;
    GtkTreeIter       iter;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

    if (gtk_tree_selection_get_selected(selection, &model, &iter))
    {
        GtkWidget * title = glade_xml_get_widget (xml, "entry_title");
        GtkWidget * command = glade_xml_get_widget (xml, "entry_command");
        GtkWidget * icon = glade_xml_get_widget (xml, "entry_icon");

        gchar *name;

        gtk_tree_model_get (model, &iter, TREE_COLUMN_TITLE, &name, -1);
        gtk_entry_set_text (GTK_ENTRY(title), name);
        g_free(name);

        gtk_tree_model_get (model, &iter, TREE_COLUMN_COMMAND, &name, -1);
        gtk_entry_set_text (GTK_ENTRY(command), name);
        g_free(name);

        gtk_tree_model_get (model, &iter, TREE_COLUMN_ICON_NAME, &name, -1);
        gtk_entry_set_text (GTK_ENTRY(icon), name);
        g_free(name);

        GtkWidget * dialog = glade_xml_get_widget (xml, "dialog_app");
        gtk_window_set_title (GTK_WINDOW (dialog), _("Edit"));
        g_signal_connect (G_OBJECT (dialog), "delete-event", G_CALLBACK (widget_hide), NULL);
        gtk_widget_show (dialog);
    }
}

void on_settings_delete()
{
    GtkWidget * treeview = glade_xml_get_widget (xml, "treeview_app");

    GtkTreeSelection *selection;
    GtkTreeModel     *model;
    GtkTreeIter       iter;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

    if (gtk_tree_selection_get_selected(selection, &model, &iter))
    {
        gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
        set_status_button();
    }
}

void on_settings_update()
{
    Config config;
    std::string filename = configFile.empty() ? config.getFile() : configFile;
    std::ofstream file(filename.c_str());

    if (!file.is_open())
    {
        GtkWidget * dialog = gtk_message_dialog_new (GTK_WINDOW(glade_xml_get_widget (xml, "dialog_settings")),
                                  GTK_DIALOG_DESTROY_WITH_PARENT,
                                  GTK_MESSAGE_ERROR,
                                  GTK_BUTTONS_OK,
                                  "%s",(_("Error save config in ") + filename).c_str());

        gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);
    }

    GtkWidget * spinbutton;
    GtkWidget * entry;

    std::string imageName, fontName;
    int fontSize;

    spinbutton = glade_xml_get_widget (xml, "spinbutton_font_size");
    fontSize = gtk_spin_button_get_value (GTK_SPIN_BUTTON (spinbutton));

    entry = glade_xml_get_widget (xml, "entry_font_name");
    fontName = gtk_entry_get_text (GTK_ENTRY (entry));

    entry = glade_xml_get_widget (xml, "entry_image_name");
    imageName = gtk_entry_get_text (GTK_ENTRY (entry));
 
    std::string fullcommand = getCommand();

    file << "i: " << imageName << std::endl;
    file << "c: " << fullcommand << std::endl;
    file << "t: " << fontName << "/" << fontSize << std::endl;
    file << std::endl;

    GtkTreeIter  iter;
    gboolean     valid;

    GtkWidget * treeview = glade_xml_get_widget (xml, "treeview_app");
    GtkListStore *list_store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (treeview)));

    valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(list_store), &iter);

    while (valid)
    {
        gchar *icon, *command, *title;

        gtk_tree_model_get (GTK_TREE_MODEL(list_store), &iter, TREE_COLUMN_ICON_NAME, &icon, -1);
        gtk_tree_model_get (GTK_TREE_MODEL(list_store), &iter, TREE_COLUMN_COMMAND, &command, -1);
        gtk_tree_model_get (GTK_TREE_MODEL(list_store), &iter, TREE_COLUMN_TITLE, &title, -1);

        file << "i: " << icon << std::endl;
        file << "c: " << command << std::endl;
        file << "t: " << title << std::endl;
        file << std::endl;

        g_free(icon);
        g_free(command);
        g_free(title);

        valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(list_store), &iter);
    }

    file.close();

    Run run;

    if (!configFile.empty())
    {
        fullcommand = PACKAGE_NAME;
        fullcommand += " --config ";
        fullcommand += configFile;
    }

    run.restart(fullcommand);
}

void on_settings_close()
{
    gtk_main_quit();
}

std::string getCommand()
{
    std::string command;
    command = PACKAGE_NAME;

    GtkWidget * checkbutton = NULL;
    GtkWidget * spinbutton = NULL;

    checkbutton = glade_xml_get_widget (xml, "checkbutton_bpress");

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton)))
    {
        command += " --bpress";
    }

    checkbutton = glade_xml_get_widget (xml, "checkbutton_above_desk");

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton)))
    {
        command += " --above-desk";
    }

    checkbutton = glade_xml_get_widget (xml, "checkbutton_vbar");

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton)))
    {
        command += " --vbar";
    }

    checkbutton = glade_xml_get_widget (xml, "checkbutton_nofont");

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton)))
    {
        command += " --nofont";
    }

    checkbutton = glade_xml_get_widget (xml, "checkbutton_pos");

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton)))
    {
        command += " --pos";
        GtkWidget * combo = glade_xml_get_widget (xml, "combobox_pos");

        switch((int)gtk_combo_box_get_active (GTK_COMBO_BOX (combo)))
        {
            case POS_TOP: command += " top"; break;
            case POS_BOTTOM: command += " bottom"; break;
            case POS_LEFT: command += " left"; break;
            case POS_RIGHT: command += " right"; break;
            case POS_CENTER: command += " center"; break;
            case POS_BOTTOM_LEFT: command += " bot-left"; break;
            case POS_BOTTOM_RIGHT: command += " bot-right"; break;
            case POS_TOP_LEFT: command += " top-left"; break;
            case POS_TOP_RIGHT: command += " top-right"; break;
        }
    }

    checkbutton = glade_xml_get_widget (xml, "checkbutton_grow");

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton)))
    {
        command += " --grow";
    }

    checkbutton = glade_xml_get_widget (xml, "checkbutton_offset");

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton)))
    {
        char text[10];
        command += " --offset";
        spinbutton = glade_xml_get_widget (xml, "spinbutton_offset");
        sprintf(text, " %d", (int)gtk_spin_button_get_value (GTK_SPIN_BUTTON (spinbutton)));
        command += text;
    }

    checkbutton = glade_xml_get_widget (xml, "checkbutton_noreload");

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton)))
    {
        command += " --noreload";
    }

    checkbutton = glade_xml_get_widget (xml, "checkbutton_taskbar");

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton)))
    {
        command += " --taskbar";
    }
    
    checkbutton = glade_xml_get_widget (xml, "checkbutton_rsize");

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton)))
    {
        char text[10];
        command += " --rsize";
        spinbutton = glade_xml_get_widget (xml, "spinbutton_rsize");
        sprintf(text, " %d", (int)gtk_spin_button_get_value (GTK_SPIN_BUTTON (spinbutton)));
        command += text;
    }

    checkbutton = glade_xml_get_widget (xml, "checkbutton_isize");

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton)))
    {
        char text[10];
        command += " --isize";
        spinbutton = glade_xml_get_widget (xml, "spinbutton_isize");
        sprintf(text, " %d", (int)gtk_spin_button_get_value (GTK_SPIN_BUTTON (spinbutton)));
        command += text;
    }

    checkbutton = glade_xml_get_widget (xml, "checkbutton_idist");

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton)))
    {
        char text[10];
        command += " --idist";
        spinbutton = glade_xml_get_widget (xml, "spinbutton_idist");
        sprintf(text, " %d", (int)gtk_spin_button_get_value (GTK_SPIN_BUTTON (spinbutton)));
        command += text;
    }

    checkbutton = glade_xml_get_widget (xml, "checkbutton_nanim");

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton)))
    {
        char text[10];
        command += " --nanim";
        spinbutton = glade_xml_get_widget (xml, "spinbutton_nanim");
        sprintf(text, " %d", (int)gtk_spin_button_get_value (GTK_SPIN_BUTTON (spinbutton)));
        command += text;
    }

    checkbutton = glade_xml_get_widget (xml, "checkbutton_zoomf");

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton)))
    {
        char text[10];
        command += " --zoomf";
        spinbutton = glade_xml_get_widget (xml, "spinbutton_zoomf");
        sprintf(text, " %f", (float)gtk_spin_button_get_value (GTK_SPIN_BUTTON (spinbutton)));
        command += text;
    }

    checkbutton = glade_xml_get_widget (xml, "checkbutton_jumpf");

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton)))
    {
        char text[10];
        command += " --jumpf";
        spinbutton = glade_xml_get_widget (xml, "spinbutton_jumpf");
        sprintf(text, " %f", (float)gtk_spin_button_get_value (GTK_SPIN_BUTTON (spinbutton)));
        command += text;
    }

    checkbutton = glade_xml_get_widget (xml, "checkbutton_dblclk");

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton)))
    {
        char text[10];
        command += " --dblclk";
        spinbutton = glade_xml_get_widget (xml, "spinbutton_dblclk");
        sprintf(text, " %d", (int)gtk_spin_button_get_value (GTK_SPIN_BUTTON (spinbutton)));
        command += text;
    }

    checkbutton = glade_xml_get_widget (xml, "checkbutton_balfa");

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton)))
    {
        char text[10];
        command += " --balfa";
        spinbutton = glade_xml_get_widget (xml, "spinbutton_balfa");
        sprintf(text, " %d", (int)gtk_spin_button_get_value (GTK_SPIN_BUTTON (spinbutton)));
        command += text;
    }

    checkbutton = glade_xml_get_widget (xml, "checkbutton_falfa");

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton)))
    {
        char text[10];
        command += " --falfa";
        spinbutton = glade_xml_get_widget (xml, "spinbutton_falfa");
        sprintf(text, " %d", (int)gtk_spin_button_get_value (GTK_SPIN_BUTTON (spinbutton)));
        command += text;
    }

    checkbutton = glade_xml_get_widget (xml, "checkbutton_filter");

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton)))
    {
        command += " --filter";
        GtkWidget * combo = glade_xml_get_widget (xml, "combobox_filter");

        switch((int)gtk_combo_box_get_active (GTK_COMBO_BOX (combo)))
        {
            case FILTER_NONE:    command += " 0"; break;
            case FILTER_HOVERED: command += " 1"; break;
            case FILTER_OTHERS:  command += " 2"; break;
            case FILTER_ALL:     command += " 3"; break;
        }
    }

    checkbutton = glade_xml_get_widget (xml, "checkbutton_fc");

    if(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton)))
    {
        GtkWidget * color = glade_xml_get_widget (xml, "colorbutton_fc");
        GdkColor rgb;
        gchar text[16];
        
        gtk_color_button_get_color (GTK_COLOR_BUTTON (color), &rgb);
        int alpha = gtk_color_button_get_alpha (GTK_COLOR_BUTTON (color));
        g_snprintf (text, sizeof (text), "0x%02x%02x%02x%02x", alpha >> 8, rgb.red >> 8, rgb.green >> 8, rgb.blue >> 8);
        
        command += " --fc ";
        command += text;
    }

    return command;
}

bool validate()
{
    GtkWidget * title = glade_xml_get_widget (xml, "entry_title");
    GtkWidget * command = glade_xml_get_widget (xml, "entry_command");
    GtkWidget * icon = glade_xml_get_widget (xml, "entry_icon");

    std::string text_title, text_command, text_icon;

    text_title = (gchar*)gtk_entry_get_text (GTK_ENTRY(title));
    text_command = (gchar*)gtk_entry_get_text (GTK_ENTRY(command));
    text_icon = (gchar*)gtk_entry_get_text (GTK_ENTRY(icon));

    bool resp = text_title != "" && text_command != "" && text_icon != "";

    if (!resp)
    {
        GtkWidget * dialog = gtk_message_dialog_new (GTK_WINDOW(glade_xml_get_widget (xml, "dialog_settings")),
                                  GTK_DIALOG_DESTROY_WITH_PARENT,
                                  GTK_MESSAGE_WARNING,
                                  GTK_BUTTONS_OK,
                                  _("Fields must not be empty"));

        gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);
    }

    return resp;
}

void on_app_accept()
{
    if (!validate()) return;

    GtkWidget * title = glade_xml_get_widget (xml, "entry_title");
    GtkWidget * command = glade_xml_get_widget (xml, "entry_command");
    GtkWidget * icon = glade_xml_get_widget (xml, "entry_icon");

    std::string text_title, text_command, text_icon;

    text_title = (gchar*)gtk_entry_get_text (GTK_ENTRY(title));
    text_command = (gchar*)gtk_entry_get_text (GTK_ENTRY(command));
    text_icon = (gchar*)gtk_entry_get_text (GTK_ENTRY(icon));

    GtkWidget * treeview = glade_xml_get_widget (xml, "treeview_app");
    GtkTreeModel * model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
    GtkListStore * list_store = GTK_LIST_STORE (model);
    GtkTreeIter iter;

    if (!edit)
    {
        gtk_list_store_append (list_store, &iter);
        gtk_list_store_set (list_store, &iter, \
                                TREE_COLUMN_ICON, get_icon(text_icon), \
                                TREE_COLUMN_TITLE, text_title.c_str(), \
                                TREE_COLUMN_COMMAND, text_command.c_str(), \
                                TREE_COLUMN_ICON_NAME, text_icon.c_str(), -1);
    }
    else
    {
        GtkTreeSelection * selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

        if (gtk_tree_selection_get_selected(selection, &model, &iter))
        {
            gtk_list_store_set (list_store, &iter, \
                                TREE_COLUMN_ICON, get_icon(text_icon), \
                                TREE_COLUMN_TITLE, text_title.c_str(), \
                                TREE_COLUMN_COMMAND, text_command.c_str(), \
                                TREE_COLUMN_ICON_NAME, text_icon.c_str(), -1);
        }
    }

    GtkWidget * dialog = glade_xml_get_widget (xml, "dialog_app");
    gtk_widget_hide (dialog);
}

void on_app_cancel()
{
    GtkWidget * dialog = glade_xml_get_widget (xml, "dialog_app");
    gtk_widget_hide (dialog);
}

void on_app_up()
{
    GtkWidget * treeview = glade_xml_get_widget (xml, "treeview_app");

    GtkTreeSelection *selection;
    GtkTreeModel     *model;
    GtkTreeIter       iter;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

    if (gtk_tree_selection_get_selected(selection, &model, &iter))
    {
        GtkTreePath *path = gtk_tree_model_get_path (model,&iter);
        GtkTreePath *prev = gtk_tree_path_copy (path);
        gtk_tree_path_prev(prev);

        GtkTreeIter before;
        gtk_tree_model_get_iter(model, &before, prev);
        gtk_list_store_move_before (GTK_LIST_STORE(model), &iter, &before);

        gtk_tree_path_free (path);
        gtk_tree_path_free (prev);
        set_status_button();
    }
}

void on_app_down()
{
    GtkWidget * treeview = glade_xml_get_widget (xml, "treeview_app");

    GtkTreeSelection *selection;
    GtkTreeModel     *model;
    GtkTreeIter       iter;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

    if (gtk_tree_selection_get_selected(selection, &model, &iter))
    {
        GtkTreeIter * next = gtk_tree_iter_copy (&iter);

        gtk_tree_model_iter_next (model, next);
        gtk_list_store_move_after (GTK_LIST_STORE(model), &iter, next);

        gtk_tree_iter_free (next);
        set_status_button();
    }
}

void on_icon_changed()
{
}

void on_treeview_cursor_changed ()
{
    set_status_button();
}

void set_status_button()
{
    GtkWidget * treeview = glade_xml_get_widget (xml, "treeview_app");
    GtkWidget * button_edit = glade_xml_get_widget (xml, "button_edit");
    GtkWidget * button_delete = glade_xml_get_widget (xml, "button_delete");
    GtkWidget * button_up = glade_xml_get_widget (xml, "button_up");
    GtkWidget * button_down = glade_xml_get_widget (xml, "button_down");

    GtkTreeSelection *selection;
    GtkTreeModel     *model;
    GtkTreeIter       iter;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

    if (gtk_tree_selection_get_selected(selection, &model, &iter))
    {
        GtkTreePath *path = gtk_tree_model_get_path (model,&iter);
        gint * index = gtk_tree_path_get_indices(path);

        if (index)
        {
            gtk_widget_set_sensitive(button_edit, true);
            gtk_widget_set_sensitive(button_delete, true);
            gtk_widget_set_sensitive(button_up, index[0] > 0);
            gtk_widget_set_sensitive(button_down, gtk_tree_model_iter_next(model, &iter));
            gtk_tree_path_free (path);
 
            return;
        }

        gtk_tree_path_free (path);

    }

    gtk_widget_set_sensitive(button_edit, false);
    gtk_widget_set_sensitive(button_delete, false);
    gtk_widget_set_sensitive(button_up, false);
    gtk_widget_set_sensitive(button_down, false);
}

gboolean clickTreeView(GtkWidget * widget, GdkEventButton * event, gpointer data)
{
    if (event->button == 3)
    {
        GtkWidget * menu = static_cast<GtkWidget *>(data);
        gtk_menu_popup (GTK_MENU(menu), NULL, NULL, NULL, NULL, event->button, event->time);
        return true;
    }

    return false;
}

void on_select_command()
{
    GtkWidget * dialog = glade_xml_get_widget (xml, "dialog_app");

    GtkWidget * file = gtk_file_chooser_dialog_new (_("Open exec file"),
     				      GTK_WINDOW(dialog),
     				      GTK_FILE_CHOOSER_ACTION_OPEN,
     				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
     				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
     				      NULL);

    GtkWidget * command = glade_xml_get_widget (xml, "entry_command");
    std::string filename = gtk_entry_get_text (GTK_ENTRY(command));
    gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (file), filename == "" ? "/usr/bin" : filename.c_str());

    if (gtk_dialog_run (GTK_DIALOG (file)) == GTK_RESPONSE_ACCEPT)
    {
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (file));
        gtk_entry_set_text (GTK_ENTRY(command), filename.c_str());
    }
     
    gtk_widget_destroy (file);
}

void on_select_icon()
{
    GtkWidget * dialog = glade_xml_get_widget (xml, "dialog_app");

    GtkWidget * file = gtk_file_chooser_dialog_new (_("Open image file"),
     				      GTK_WINDOW(dialog),
     				      GTK_FILE_CHOOSER_ACTION_OPEN,
     				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
     				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
     				      NULL);

    GtkWidget * icon = glade_xml_get_widget (xml, "entry_icon");
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pixbuf_formats(filter);
    gtk_file_filter_set_name (filter, _("Images"));

    std::string filename = gtk_entry_get_text (GTK_ENTRY(icon));

    gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (file), filename == "" ? PIXMAPDIR : filename.c_str());
    gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (file), filter);

    if (gtk_dialog_run (GTK_DIALOG (file)) == GTK_RESPONSE_ACCEPT)
    {
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (file));
        gtk_entry_set_text (GTK_ENTRY(icon), filename.c_str());
    }
     
    gtk_widget_destroy (file);
}

void on_select_fontname()
{
    GtkWidget * dialog = glade_xml_get_widget (xml, "dialog_settings");

    GtkWidget * file = gtk_file_chooser_dialog_new (_("Open font file"),
     				      GTK_WINDOW(dialog),
     				      GTK_FILE_CHOOSER_ACTION_OPEN,
     				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
     				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
     				      NULL);

    GtkWidget * font = glade_xml_get_widget (xml, "entry_font_name");
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name (filter, _("Fonts"));
    gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter), "*.ttf"); 

    std::string filename = gtk_entry_get_text (GTK_ENTRY(font));

    gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (file), filename.c_str());
    gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (file), filter);

    if (gtk_dialog_run (GTK_DIALOG (file)) == GTK_RESPONSE_ACCEPT)
    {
        Utils util;
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (file));
        filename = util.replace(filename, ".ttf", "");
        gtk_entry_set_text (GTK_ENTRY(font), filename.c_str());
    }
     
    gtk_widget_destroy (file);
}

void on_select_imagename()
{
    GtkWidget * dialog = glade_xml_get_widget (xml, "dialog_settings");

    GtkWidget * file = gtk_file_chooser_dialog_new (_("Open image file"),
     				      GTK_WINDOW(dialog),
     				      GTK_FILE_CHOOSER_ACTION_OPEN,
     				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
     				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
     				      NULL);

    GtkWidget * icon = glade_xml_get_widget (xml, "entry_image_name");
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pixbuf_formats(filter);
    gtk_file_filter_set_name (filter, _("Images"));

    std::string filename = gtk_entry_get_text (GTK_ENTRY(icon));

    gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (file), filename.c_str());
    gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (file), filter);

    if (gtk_dialog_run (GTK_DIALOG (file)) == GTK_RESPONSE_ACCEPT)
    {
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (file));
        gtk_entry_set_text (GTK_ENTRY(icon), filename.c_str());
    }
     
    gtk_widget_destroy (file);
}

void checkbutton_toggled (GtkToggleButton *togglebutton, gpointer data)
{
    GtkWidget * widget = static_cast<GtkWidget *>(data);
    gtk_widget_set_sensitive (widget, gtk_toggle_button_get_active (togglebutton));
}

gboolean widget_hide(GtkWidget * widget, GdkEvent * event, gpointer data)
{
    gtk_widget_hide (widget);
    return true;
}

void show_about()
{
    GdkPixbuf * pixbuf;
    pixbuf = gdk_pixbuf_new_from_file (ICON_DEFAULT, NULL);

    GtkWidget * settings = glade_xml_get_widget (xml, "dialog_settings");

    gtk_show_about_dialog ( gtk_widget_get_tooltip_window (settings),
        "title", _("About"),
        "icon", pixbuf,
        "program-name", _("wbar config"),
        "version", VERSION,
        "copyright", _("License GPL - 2010"),
        "comments", _("A light and fast launch bar."),
        "website", "http://code.google.com/p/wbar/",
        "website-label", _("wbar project website"),
        "authors", authors,
        "documenters", documenters,
        "translator-credits", translater,
        "logo", pixbuf,
        NULL);
}

