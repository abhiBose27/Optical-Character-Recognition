#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "err.h"
#include "gtk/gtk.h"
#include "gtk/gtkx.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "sdl_libs.h"
#include "Network.h"
#include "TextDestruction.h"
#include "TextReconstruction.h"
#include "pixeloperations.h"
#include "PreProcessing.h"


char* file_name;
GtkWidget* image_widget;

typedef struct open_image_widgets {
    GtkWidget* window;
    GtkWidget* frame;
    GtkWidget* run_button;
} oi_widgets;

typedef struct run_image_widgets {
    GtkWidget* window;
} ri_widgets;

void load_image_to_frame(char* file_name, GtkWidget* frame){
    GdkPixbuf* pixbuf_mini = NULL;
    GdkPixbuf* pixbuf = NULL;
    GError* error = NULL;
    if (image_widget)
        gtk_container_remove(GTK_CONTAINER(frame), image_widget);
    pixbuf = gdk_pixbuf_new_from_file(file_name, &error);
    if (!error){
        if (gdk_pixbuf_get_width(pixbuf) >= 990 || gdk_pixbuf_get_height(pixbuf) >= 590)
            pixbuf_mini = gdk_pixbuf_scale_simple(pixbuf, gdk_pixbuf_get_width(pixbuf) / 2, gdk_pixbuf_get_height (pixbuf) / 2, GDK_INTERP_NEAREST);
        pixbuf_mini = gdk_pixbuf_scale_simple(pixbuf, gdk_pixbuf_get_width(pixbuf), gdk_pixbuf_get_height(pixbuf), GDK_INTERP_NEAREST);
        image_widget = gtk_image_new_from_pixbuf(pixbuf_mini);
        gtk_container_add(GTK_CONTAINER(frame), image_widget);
        gtk_widget_show(image_widget);
    }
}

void open_image(GtkButton* button, oi_widgets* widgets){
    GtkWidget* dialog = gtk_file_chooser_dialog_new("Choose an image ", GTK_WINDOW(widgets->window), GTK_FILE_CHOOSER_ACTION_OPEN, "_Open", GTK_RESPONSE_OK,"_Close", GTK_RESPONSE_CANCEL, NULL);
    GtkFileFilter* filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filter, "*.bmp");
    gtk_file_filter_add_pattern(filter, "*.png");
    gtk_file_filter_add_pattern(filter, "*.jpeg");
    gtk_file_filter_add_pattern(filter, "*.jpg");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
    gtk_widget_show_all(dialog);
    gint resp = gtk_dialog_run(GTK_DIALOG(dialog));
    if (resp == GTK_RESPONSE_OK){
        file_name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        load_image_to_frame(file_name, widgets->frame);
        gtk_widget_set_sensitive(widgets->run_button, TRUE);
    }
    gtk_widget_destroy(dialog);
}

void run_image(GtkButton* button, ri_widgets* widgets){
    if (init_SDL() == -1)
        errx(1, "Could not initialize SDL: %s. \n", SDL_GetError()); 

    SDL_Surface* image = get_image(file_name);
    if (!image)
        errx(1, "Cant get image from %s: %s", file_name, SDL_GetError());

    SDL_Surface* new_image = pre_processing(image);
    text_destruction(new_image);
    display_image(new_image, 2000);
    text_reconstruction(new_image);

    GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(widgets->window), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE,
      "The text has been saved in the current directory. Named as: Text");
    gtk_window_set_title(GTK_WINDOW(dialog), "Warning");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    SDL_FreeSurface(image);
    SDL_FreeSurface(new_image);
    SDL_Quit();
}

//Main function

int main(int argc, char* argv[]){
    //Init GTK
    gtk_init(&argc, &argv);

    //Extracting all the widgets from the UI_Design.glade
    GtkBuilder* builder = gtk_builder_new_from_file ("UI_Design.glade");
    GtkWidget* window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    GtkWidget* open_button = GTK_WIDGET(gtk_builder_get_object(builder, "open"));
    GtkWidget* run_button = GTK_WIDGET(gtk_builder_get_object(builder, "Run_button"));
    GtkWidget* frame = GTK_WIDGET(gtk_builder_get_object(builder, "frame"));
    gtk_widget_set_sensitive(run_button, FALSE);

    // Open image widgets init
    oi_widgets* o_widgets = malloc(sizeof(oi_widgets));
    o_widgets->frame = frame;
    o_widgets->window = window;
    o_widgets->run_button = run_button;

    // Run image widgets init
    ri_widgets* r_widgets = malloc(sizeof(ri_widgets));
    r_widgets->window = window;

    //signal function
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(open_button, "clicked", G_CALLBACK(open_image), o_widgets);
    g_signal_connect(run_button, "clicked", G_CALLBACK(run_image), r_widgets);

    //Display the window
    gtk_widget_show(window);

    //GTK main loop
    image_widget = NULL;
    gtk_main();
    free(r_widgets);
    free(o_widgets);
    return EXIT_SUCCESS;
}
