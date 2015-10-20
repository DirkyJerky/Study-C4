#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "common.h"
#define MAX_BUF 512

GtkEntry *displayTextEntry;
GtkLabel *displayLabel;
gboolean on_displayTextEntry_changed(GtkEntry *entry, gpointer user_data) {
    gtk_label_set_text(displayLabel, gtk_entry_get_text(entry));

    return TRUE;
}

gboolean on_fontButton_font_set(GtkFontButton *button, gpointer user_data) {
    PangoFontDescription *fontDesc = pango_font_description_from_string(
            gtk_font_button_get_font_name(button));

    PangoAttribute *fontAttr = pango_attr_font_desc_new(fontDesc);

    PangoAttrList *attrList = pango_attr_list_new();

    pango_attr_list_insert(attrList, fontAttr);

    gtk_label_set_attributes(displayLabel, attrList);
    return TRUE;
}

int main(int argc, char *argv[]) {
    GtkBuilder *builder;
    GtkWidget *window;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();

    GError *err = NULL;

    if(gtk_builder_add_from_file(builder, "gladeFiles/LessonTen.glade", &err) == 0) {
        g_error(err->message);
        return EXIT_FAILURE;
    }

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
    displayTextEntry = GTK_ENTRY(gtk_builder_get_object(builder, "displayTextEntry"));
    displayLabel = GTK_LABEL(gtk_builder_get_object(builder, "displayLabel"));

    gtk_builder_connect_signals(builder, NULL);

    g_object_unref(G_OBJECT(builder));

    gtk_widget_show(window);
    gtk_main();

    return EXIT_SUCCESS;
}
