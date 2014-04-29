#include "header.h"

#define COMMAND_LEN 20
#define DATA_SIZE 512

/*
Function for switching active window
*/
void WindowSwitcher(Bitwise WindowSwitcherFlag)
{
	if(config_checking() != 1)
	{
		gtk_main_quit();
	}
		
	/*main menu window switcher*/
	(f_main_window == TRUE)?gtk_widget_show(mainmenuwindow->window):gtk_widget_hide(mainmenuwindow->window);
	
	/*registration window switcher*/
	(f_registration_window == TRUE)?gtk_widget_show(registrationwindow->window):gtk_widget_hide(registrationwindow->window);

	/*new trans window switcher*/
	if(f_new_nfc_window == TRUE)
	{
		memset(&lastAbsentData,0,sizeof(lastAbsentData));
		
		gchar SESN_text[4];
		int randomnumber;
		randomnumber = random_number_generator(100,999);
		snprintf(SESN_text,  4, "%d", randomnumber);
		gtk_label_set_text((GtkLabel *)newNFCwindow->SESN_label, SESN_text);
		
		lastAbsentData.SESNint = randomnumber;
		lastAbsentData.SESNbyte[0] = (randomnumber>>8) & 0xFF;
		lastAbsentData.SESNbyte[1] = randomnumber & 0xFF;
		gtk_widget_show(newNFCwindow->window);
		
		//NFC polling goes in here
		nfc_poll_child_process(SESN_text);
	}
	else
	{
		gtk_widget_hide(newNFCwindow->window);	
	}
		
	/*new trans QR window switcher*/
	if(f_new_qr_window == TRUE)
	{
		memset(&lastAbsentData,0,sizeof(lastAbsentData));

		gchar SESN_text[4];
		int randomnumber;
		randomnumber = random_number_generator(100,999);
		snprintf(SESN_text,  4, "%d", randomnumber);
		gtk_label_set_text((GtkLabel *)newQRwindow->SESN_label, SESN_text);
		
		lastAbsentData.SESNint = randomnumber;
		lastAbsentData.SESNbyte[0] = (randomnumber>>8) & 0xFF;
		lastAbsentData.SESNbyte[1] = randomnumber & 0xFF;
		
		gtk_widget_show(newQRwindow->window);
	}
	else
	{
		if(remove("merch_req.png") == 0)
			printf("merch_req.png deleted!\n");
			
		gtk_widget_hide(newQRwindow->window);	
	}
}
