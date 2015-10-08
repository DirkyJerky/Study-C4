#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "common.h"
#define MAX_BUF 512

gboolean on_%%%(GtkWidget *widget, gpointer user_data) {
    return TRUE;
}

int main(int argc, char *argv[]) {
    GtkBuilder *builder;
    GtkWidget *window;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();

    GError *err = NULL;

    if(gtk_builder_add_from_file(builder, "gladeFiles/Lesson%%%.glade", &err) == 0) {
        g_error(err->message);
        return EXIT_FAILURE;
    }

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));

    gtk_builder_connect_signals(builder, NULL);

    g_object_unref(G_OBJECT(builder));

    gtk_widget_show(window);
    gtk_main();

    return EXIT_SUCCESS;
}
