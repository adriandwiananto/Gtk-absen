#include "header.h"

#define DECLARE_VARIABLES
	
int main(int argc, char *argv[])
{	
	/* global variable initialization */
	memset(&lastAbsentData,0,sizeof(lastAbsentData));
	
	/* local variable declaration */
	Bitwise WindowSwitcherFlag;
	int config_status = 0; // -1 = config tampered!, 1 = config ok, 2 = config empty
	
	/* allocate memory to global struct */
	mainmenuwindow = g_slice_new(MainMenuWindow);
	registrationwindow = g_slice_new(RegistrationWindow);
	newNFCwindow = g_slice_new(NewAbsentWindow);
	newQRwindow = g_slice_new(NewAbsentWindow);
	sendingWindow = g_slice_new(SendingWindow);
	
	/* check config integrity */
	config_status = config_checking();
	
	/* gtk start */
	gtk_init(&argc,&argv);

	/* create all gtk window */
	if(init_mainmenu_window() == FALSE) return 1;
	if(init_registration_window() == FALSE) return 1;
	if(init_newnfc_window() == FALSE) return 1;
	if(init_newqr_window() == FALSE) return 1;
	if(init_sending_window() == FALSE) return 1;
	
	/* open gtk window according to the result of config file checking */
	switch(config_status)
	{
		case -1:
			error_message("unauthorized config file modification is detected!!");
			return -1;
			break;
		case 1:
			f_status_window = FALSE;	//hide all window
			f_main_window = TRUE;	//show password window
			WindowSwitcher(WindowSwitcherFlag);
			break;
		case 2:
			printf("no config file, registration process called\n");
			f_status_window = FALSE;		//hide all window
			f_registration_window = TRUE;	//show registration window
			WindowSwitcher(WindowSwitcherFlag);
			break;
	}
	
	/* start gtk */		
	gtk_main();

	printf("exit from gtk main loop\n");
	
	/* free memory for global struct */
	g_slice_free(MainMenuWindow, mainmenuwindow);
	g_slice_free(RegistrationWindow, registrationwindow);
	g_slice_free(NewAbsentWindow, newQRwindow);
	g_slice_free(NewAbsentWindow, newNFCwindow);
	g_slice_free(SendingWindow, sendingWindow);
	
	return 0;
}

/*
We call error_message() any time we want to display an error message to the
user. It will both show an error dialog and log the error to the terminal
window.
*/
void
error_message (const gchar *message)
{
        GtkWidget               *dialog;
        
        /* log to terminal window */
        g_warning ("%s",message);
        
        /* create an error message dialog and display modally to the user */
        dialog = gtk_message_dialog_new (NULL, 
                                         GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_MESSAGE_ERROR,
                                         GTK_BUTTONS_OK,
                                         "%s", message);
        
        gtk_window_set_title (GTK_WINDOW (dialog), "Error!");
        gtk_dialog_run (GTK_DIALOG (dialog));      
        gtk_widget_destroy (dialog);         
}

/*
We call notification_message() any time we want to display a notification to the
user.
*/
void
notification_message (const gchar *message)
{
        GtkWidget               *dialog;
             
        /* create an error message dialog and display modally to the user */
        dialog = gtk_message_dialog_new (NULL, 
                                         GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_MESSAGE_INFO,
                                         GTK_BUTTONS_CLOSE,
                                         "%s", message);
        
        gtk_window_set_title (GTK_WINDOW (dialog), "Notification");
        gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);
}
