#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "common.h"
#define MAX_BUF 512

GtkWidget *window;
GtkSpinButton *spinButton;
GtkDrawingArea *dArea;
static cairo_surface_t *backMap = NULL;

gdouble startX;
gdouble startY;

void clear_surface(void);
void doDrawing(gdouble, gdouble);

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

    gtk_widget_queue_draw(GTK_WIDGET(dArea));

    return TRUE;
}

void clear_surface() {
    cairo_t *cr;

    cr = cairo_create (backMap);

    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_paint (cr);

    cairo_destroy (cr);
}

void doDrawing(gdouble endX, gdouble endY) {
    cairo_t *cr = cairo_create(backMap);
    // gdk_cairo_set_source_rgba(cr, color);
    // cairo_set_line_width(cr, (gdouble) gtk_spin_button_get_value_as_int(spinButton));
    // cairo_move_to(cr, xS, yS);
    // cairo_line_to(cr, xE, yE);
    // cairo_stroke(cr);
    cairo_destroy(cr);

    // g_debug("Line: (%f, %f) -> (%f, %f) Color: %s", xS, yS, xE, yE, gdk_rgba_to_string(color));

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
    spinButton = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton1"));
    dArea = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "drawingarea1"));

    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);
}

void setupObjects() {
    //backMap = gdk_window_create_similar_image_surface(gtk_widget_get_window(GTK_WIDGET(dArea)), 
    //        CAIRO_FORMAT_ARGB32,
    //        gtk_widget_get_allocated_height(GTK_WIDGET(dArea)), 
    //        gtk_widget_get_allocated_width(GTK_WIDGET(dArea)),
    //        1);

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