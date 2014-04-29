#include "header.h"
//~ #include <dirent.h>

//~ static pid_t proc_find(const char* name);

/*
We call init_mainmenu_window() when our program is starting to load 
main menu window with references to Glade file. 
*/
gboolean init_newqr_window()
{
	GtkBuilder              *builder;
	GError                  *err=NULL;

	/* use GtkBuilder to build our interface from the XML file */
	builder = gtk_builder_new ();
	if (gtk_builder_add_from_file (builder, UI_GLADE_FILE, &err) == 0)
	{
		error_message (err->message);
		g_error_free (err);
		return FALSE;
	}

	/* get the widgets which will be referenced in callbacks */
	newQRwindow->window = GTK_WIDGET (gtk_builder_get_object (builder, "new_qr_window"));
	newQRwindow->SESN_label = GTK_WIDGET (gtk_builder_get_object (builder, "new_qr_SESN_label"));

	gtk_builder_connect_signals (builder, newQRwindow);
	g_object_unref(G_OBJECT(builder));
	
	return TRUE;
}

void on_new_qr_continue_button_clicked()
{
	printf("continue clicked\n");
}

void on_new_qr_cancel_button_clicked()
{
	printf("cancel clicked\n");
	Bitwise WindowSwitcherFlag;
	f_status_window = FALSE;	//hide all window
	f_main_window = TRUE;		//show password window
	WindowSwitcher(WindowSwitcherFlag);
}
