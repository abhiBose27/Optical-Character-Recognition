#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "gtk/gtk.h"
#include "gtk/gtkx.h"
#include <math.h>
#include <ctype.h>
#include "Network.h"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include"CropImage.h"
#include"CharDetection.h"
#include"BlackAndWhite.h"
#include "pixeloperations.h"
#include "GaussianBlur.h"
#include "deskew.h"
#include "err.h"
#include "sdl_libs.h"
//Widgets

GtkWidget *image;
GtkWidget *frame;
GtkWidget *run_button;
GtkWidget *window;
char * file_name;
void image_load (char *file_name);

void open_image(GtkButton *Button, gpointer window)
{

  GtkWidget *dialog;
  GtkFileFilter *filter;
  dialog = gtk_file_chooser_dialog_new("Choose an image ", GTK_WINDOW(window),GTK_FILE_CHOOSER_ACTION_OPEN,"_Open",
  GTK_RESPONSE_OK,"_Close",GTK_RESPONSE_CANCEL,NULL);
  filter = gtk_file_filter_new();
  gtk_file_filter_add_pattern(filter, "*.bmp");
  gtk_file_filter_add_pattern(filter, "*.png");
  gtk_file_filter_add_pattern(filter, "*.jpeg");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
  gtk_widget_show_all(dialog);
  gint resp = gtk_dialog_run(GTK_DIALOG(dialog));
  if (resp == GTK_RESPONSE_OK)
  {
    file_name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    image_load(file_name);
    gtk_widget_set_sensitive(run_button, TRUE);
  }
  gtk_widget_destroy(dialog);
}

void image_load(char * file_name)
{
  GdkPixbuf *pixbuf_mini = NULL;
  GdkPixbuf *pixbuf = NULL;
  GError *error = NULL;
  if (image)
    gtk_container_remove (GTK_CONTAINER (frame), image);
  pixbuf = gdk_pixbuf_new_from_file (file_name, &error);
  if (!error)
  {
    if ( gdk_pixbuf_get_width (pixbuf) >= 990 || gdk_pixbuf_get_height (pixbuf) >= 590)
    {
      pixbuf_mini = gdk_pixbuf_scale_simple (pixbuf,
                                               gdk_pixbuf_get_width (pixbuf) / 2,
                                               gdk_pixbuf_get_height (pixbuf) / 2,
                                               GDK_INTERP_NEAREST);
    }
    pixbuf_mini = gdk_pixbuf_scale_simple (pixbuf,
                                             gdk_pixbuf_get_width (pixbuf),
                                             gdk_pixbuf_get_height (pixbuf),
                                             GDK_INTERP_NEAREST);
    image = gtk_image_new_from_pixbuf (pixbuf_mini);
    gtk_container_add (GTK_CONTAINER (frame), image);
    gtk_widget_show(image);


  }
}

SDL_Surface* PreProcessing(SDL_Surface* img)
{
  SDL_Surface* newImage;
  Contrast(img, 40);
  grayScale(img);
  Convolution(img);
  Binarise(img);
  newImage = ChipTheEdges(img);
  newImage = auto_deskew(newImage);
  return newImage;

  
}

void run_program(GtkButton *Button, gpointer window)
{
    SDL_Surface *image;
    SDL_Surface* newImage;
    init_SDL();
    image = load_image(file_name);
    if (!image)
        errx(1, "null surface was given, try again with appropriate image");
    newImage = PreProcessing(image);
    SDL_Surface** surfaces;
    surfaces = DetectCharacter(newImage);
    int nb_characters = NbCharacters();
    text_conversion(surfaces, nb_characters);
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE,
      "The text has been saved in the current directory. Named as: Result");
    gtk_window_set_title(GTK_WINDOW(dialog), "Warning");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return;
}

//Main function

int main(int argc, char *argv[])
{

  GtkWidget *open_button;
  GtkBuilder *builder;


  //Init GTK

  gtk_init (&argc, &argv);

  //Extracting all the widgets from the UI_Design.glade

  builder = gtk_builder_new_from_file ("UI_Design.glade");
  window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
  open_button = GTK_WIDGET(gtk_builder_get_object(builder, "open"));
  run_button = GTK_WIDGET(gtk_builder_get_object(builder, "Run_button"));
  frame = GTK_WIDGET(gtk_builder_get_object(builder, "frame"));
  gtk_widget_set_sensitive(run_button, FALSE);

  //signal function
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(open_button, "clicked", G_CALLBACK(open_image), window);
  g_signal_connect(run_button, "clicked", G_CALLBACK(run_program), window);

  //Display the window
  gtk_widget_show(window);

  //GTK main loop
  image = NULL;
  gtk_main();

  return EXIT_SUCCESS;



}
