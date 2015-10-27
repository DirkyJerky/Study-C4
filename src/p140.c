#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "common.h"
#define MAX_BUF 512

GtkWidget *window;
GtkWidget *dArea;
GtkWidget *colorButton;
GtkWidget *colorMenuItem;
GtkWidget *widthItem1;
GtkWidget *widthItem2;
GtkWidget *widthItem3;
GtkWidget *widthItem4;
GtkWidget *widthItem5;
GtkWidget *shapeItem1;
GtkWidget *shapeItem2;
GtkWidget *shapeItem3;
GtkWidget *shapeItem4;
GtkWidget *lineItem1;
GtkWidget *lineItem2;
GtkWidget *lineItem3;

static cairo_surface_t *backMap = NULL;

gdouble startX;
gdouble startY;

typedef enum { LINE, RECTANGLE, ECLIPSE, DIAMOND } shape_t;

shape_t selectedType = LINE;

void clear_surface(void);
void doDrawing(gdouble, gdouble);

void on_colorMenuItem_activate(GtkWidget *widget, gpointer user_data) {
    g_debug("on_colorMenuItem_activate");
    gtk_button_clicked(GTK_BUTTON(colorButton));
}

//void on_colorMenuItem_activate_item(GtkWidget *widget, gpointer user_data) {
//    g_debug("on_colorMenuItem_activate_item");
//    gtk_button_clicked(GTK_BUTTON(colorButton));
//}

gboolean on_drawingarea1_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    g_debug("on_drawingarea1_draw");

    cairo_set_source_surface(cr, backMap, 0, 0);
    cairo_paint(cr);

    return FALSE;
}

gboolean on_drawingarea1_button_press_event(GtkWidget *widget, GdkEvent *gEvent, gpointer user_data) {
    GdkEventButton event = gEvent->button;

    g_debug("on_drawingarea1_button_press_event with Button: %d", event.button);

    if(event.type != GDK_BUTTON_PRESS || event.button != 1) return FALSE;

    startX = event.x;
    startY = event.y;

    return TRUE;
}

gboolean on_drawingarea1_configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer data) {
    g_debug("on_drawingarea1_configure_event");
    if (backMap) {
        cairo_surface_destroy (backMap);
    }

    backMap = gdk_window_create_similar_surface (gtk_widget_get_window (widget),
            CAIRO_CONTENT_COLOR_ALPHA,
            gtk_widget_get_allocated_width (widget),
            gtk_widget_get_allocated_height (widget));

    g_debug("backMap status: %s", cairo_status_to_string(cairo_surface_status(backMap)));

    // Initialize the surface to white
    clear_surface();

    // We've handled the configure event, no need for further processing.
    return TRUE;
}

gboolean on_drawingarea1_button_release_event(GtkWidget *widget, GdkEvent *gEvent, gpointer user_data) {
    GdkEventButton event = gEvent->button;

    g_debug("on_drawingarea1_button_release_event with Button: %d", event.button);

    if(event.type != GDK_BUTTON_RELEASE || event.button != 1) return FALSE;

    doDrawing(event.x, event.y);

    gtk_widget_queue_draw(dArea);

    return TRUE;
}

void clear_surface() {
    cairo_t *cr;

    cr = cairo_create (backMap);

    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_paint (cr);

    cairo_destroy (cr);
}

void setForContext(cairo_t *cr, GtkRadioMenuItem *firstItem) {
    GSList *itemList = gtk_radio_menu_item_get_group(firstItem);
    GtkCheckMenuItem *tmpCheckItem = NULL;
    const char *selectedName = NULL;
    while(itemList) {
        tmpCheckItem = itemList->data;
        itemList = itemList->next;
        if(gtk_check_menu_item_get_active (tmpCheckItem)) {
            selectedName = gtk_widget_get_name(GTK_WIDGET(tmpCheckItem));
        }
    }

    if (selectedName == NULL) return;

    g_debug("Name: %s", selectedName);
    static int lineWidth;

    // SWITCH OVER ALL NAMES
    if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widthItem1))) {
        cairo_set_line_width(cr, 1.0);
        lineWidth = 1;
    }

    if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widthItem2))) {
        cairo_set_line_width(cr, 8.0);
        lineWidth = 8;
    }

    if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widthItem3))) {
        cairo_set_line_width(cr, 16.0);
        lineWidth = 16;
    }

    if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widthItem4))) {
        cairo_set_line_width(cr, 24.0);
        lineWidth = 24;
    }

    if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widthItem5))) {
        cairo_set_line_width(cr, 32.0);
        lineWidth = 32;
    }

    g_debug("lineWidth = %d", lineWidth);

    if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(lineItem1))) {
        cairo_set_dash(cr, NULL, 0, 0);
        cairo_set_line_cap(cr, CAIRO_LINE_CAP_BUTT);

        g_debug("lineItem1 active and set");
    } 
    
    if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(lineItem2))) {
        double dashes[2];
        dashes[0] = 0.0;
        dashes[1] = lineWidth * 2;
        cairo_set_dash(cr, dashes, 2, 0);
        cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

        g_debug("lineItem2 active and set");
    } 
    
    if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(lineItem3))) {
        double dashes[2];
        dashes[0] = dashes[1] = lineWidth * 2;
        cairo_set_dash(cr, dashes, 2, 0);
        cairo_set_line_cap(cr, CAIRO_LINE_CAP_BUTT);

        g_debug("lineItem3 active and set");
    } 
    
    if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(shapeItem1))) {
        selectedType = LINE;

        g_debug("selectedType = LINE");
    } 
    
    if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(shapeItem2))) {
        selectedType = RECTANGLE;

        g_debug("selectedType = RECTANGLE");
    } 
    
    if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(shapeItem3))) {
        selectedType = ECLIPSE;

        g_debug("selectedType = ECLIPSE");
    } 
    
    if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(shapeItem4))) {
        selectedType = DIAMOND;

        g_debug("selectedType = DIAMOND");
    } 
}

cairo_t *setupContext() {
    cairo_t *cr = cairo_create(backMap);
    setForContext(cr, GTK_RADIO_MENU_ITEM(widthItem1));
    setForContext(cr, GTK_RADIO_MENU_ITEM(shapeItem1));
    setForContext(cr, GTK_RADIO_MENU_ITEM(lineItem1));

    GdkRGBA colorButtonColor;
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(colorButton), &colorButtonColor);
    gdk_cairo_set_source_rgba(cr, &colorButtonColor);

    return cr;
}

void doDrawing(gdouble endX, gdouble endY) {
    cairo_t *cr = setupContext();

    switch(selectedType) {
        case LINE:
            cairo_move_to(cr, startX, startY);
            cairo_line_to(cr, endX, endY);
            cairo_stroke(cr);
            break;

        case RECTANGLE:
            cairo_move_to(cr, startX, startY);
            cairo_line_to(cr, startX, endY);
            cairo_line_to(cr, endX, endY);
            cairo_line_to(cr, endX, startY);
            cairo_close_path(cr);
            cairo_fill(cr);
            break;
            
        case ECLIPSE:
            cairo_save(cr); // Save current trans matrix

            cairo_translate(cr, (startX + endX)/2, (startY + endY)/2); // Set origin as center of ellipse
            cairo_scale(cr, (startX - endX) / 2.0, (startY - endY) / 2.0); // Strech matrix so our circle will people an ellipse
            cairo_arc(cr, 0.0, 0.0, 1.0, 0.0, 2 * M_PI); // Make the "circle"
            cairo_fill(cr);

            cairo_restore(cr); // Restore old matrix
            break;

        case DIAMOND:
            cairo_move_to(cr, (startX + endX)/2.0, startY); // Bottom
            cairo_line_to(cr, startX, (startY + endY)/2.0); // Left
            cairo_line_to(cr, (startX + endX)/2.0, endY); // Top
            cairo_line_to(cr, endX, (startY + endY)/2.0); // Right
            cairo_close_path(cr);
            cairo_fill(cr);
            break;
    }

    cairo_destroy(cr);

    //  cairo_surface_write_to_png(backMap, "image.png");
}


void setupBuilder() {
    GtkBuilder *builder;
    builder = gtk_builder_new();

    GError *err = NULL;
    if(gtk_builder_add_from_file(builder, "gladeFiles/LessonFourteen.glade", &err) == 0) {
        g_critical(err->message);
    }

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
    dArea = GTK_WIDGET(gtk_builder_get_object(builder, "drawingarea1"));
    BUILDER_GET(colorButton)
    BUILDER_GET(colorMenuItem)

    BUILDER_GET(widthItem1)
    BUILDER_GET(widthItem2)
    BUILDER_GET(widthItem3)
    BUILDER_GET(widthItem4)
    BUILDER_GET(widthItem5)
    BUILDER_GET(shapeItem1)
    BUILDER_GET(shapeItem2)
    BUILDER_GET(shapeItem3)
    BUILDER_GET(shapeItem4)
    BUILDER_GET(lineItem1)
    BUILDER_GET(lineItem2)
    BUILDER_GET(lineItem3)

    g_object_ref_sink(colorButton);

    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);
}

void helperRemoveChild(GtkWidget *widget, gpointer data) {
    gtk_container_remove(GTK_CONTAINER(colorMenuItem), widget);
}

void setupObjects() {
    gtk_container_foreach(GTK_CONTAINER(colorMenuItem), helperRemoveChild, NULL);

    g_object_ref(colorButton);
    gtk_container_add(GTK_CONTAINER(colorMenuItem), colorButton);
    g_object_unref(colorButton);
}

int main(int argc, char *argv[]) {
    g_log_set_fatal_mask("all", G_LOG_LEVEL_CRITICAL);
    gtk_init(&argc, &argv);

    setupBuilder();

    setupObjects();

    gtk_widget_show(window);
    gtk_main();

    return EXIT_SUCCESS;
}
