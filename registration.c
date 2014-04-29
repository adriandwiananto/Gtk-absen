#include "header.h"

/* abort registration, delete config and log file */
static void abort_registration()
{
	if(remove("config.cfg") == 0)
		printf("config.cfg deleted!\n");
	else
		printf("config.cfg not exists\n");
		
	error_message("Registration failed! Please retry registration process");
	exit(1);
}

static gboolean send_regData(uintmax_t ACCN, int HWID)
{
	json_object *jobj = create_registration_json(ACCN,HWID);
	
	printf("json object in string: %s\n",json_object_to_json_string(jobj));
	
	return send_reg_jsonstring_to_server 	(json_object_to_json_string(jobj), 
											"https://emoney-server.herokuapp.com/register.json");
}

/*
We call init_registration_window() when our program is starting to load 
settlement window with references to Glade file. 
*/
gboolean init_registration_window()
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
	registrationwindow->window = GTK_WIDGET (gtk_builder_get_object (builder, "registration_window"));
	registrationwindow->ACCN_entry = GTK_WIDGET (gtk_builder_get_object (builder, "registration_ACCN_entry"));

	gtk_builder_connect_signals (builder, registrationwindow);
	g_object_unref(G_OBJECT(builder));
	
	return TRUE;
}

void on_registration_ACCN_entry_insert_text(GtkEditable *buffer, gchar *new_text, gint new_text_length, gint *position, gpointer data)
{
	int i;
	guint sigid;

	/* Only allow 0-9 to be written to the entry */
	for (i = 0; i < new_text_length; i++) {
		if (new_text[i] < '0' || new_text[i] > '9') {
			sigid = g_signal_lookup("insert-text",
						G_OBJECT_TYPE(buffer));
			g_signal_stop_emission(buffer, sigid, 0);
			return;
		}
	}
}

/* Callback for Request button in registration window */
void on_registration_request_button_clicked()
{
	printf("request pressed!\n");
	const gchar *new_ACCN_entry;

	new_ACCN_entry = gtk_entry_get_text(GTK_ENTRY(registrationwindow->ACCN_entry));

	if(strcmp(new_ACCN_entry, ""))
	{
		/*convert ACCN type from string to long int (64 byte)*/
		uintmax_t ACCN;
		ACCN = strtoumax(new_ACCN_entry, NULL, 10);
		
		/*make sure ACCN value is not greater than maximum of 6 bytes value*/
		if (ACCN >= 0xFFFFFFFFFFFF)
		{
			error_message("Account ID value error");
			gtk_entry_set_text((GtkEntry *)registrationwindow->ACCN_entry, "");
		}
		else /*user input valid data to all text entry*/
		{
			char HWID[16];
			memset(HWID,0,16);
			if(get_USB_reader_HWID(HWID) == TRUE)
			{
				int HWIDint = strtoimax(HWID,NULL,10);

				if(send_regData(ACCN, HWIDint) == FALSE)
					abort_registration();
					
				/*create new config file (with error checking)*/
				if(create_new_config_file(ACCN, HWID) == FALSE)
				{
					error_message("Error creating config file");
					abort_registration();
				}

				notification_message("Registration Success! Restart the application");

				gtk_main_quit();
			}
			else
			{
				error_message("Connect USB reader!");
			}
		}
	}
}

/* Callback for Cancel button in registration window */
void on_registration_cancel_button_clicked()
{
	printf("cancel pressed!\n");
	gtk_main_quit();
}

gboolean get_USB_reader_HWID (char* hwid)
{
	FILE *p;
	char popenData[32];
	memset(popenData,0,32);
	char detectSTR[6];
	memset(detectSTR,0,6);

	p = popen("./picc_emulation_write hwid", "r");

	if(!p) 
	{
		fprintf(stderr, "Error opening pipe.\n");
		return FALSE;
	}

	while(!feof(p)) 
	{
		fgets(popenData, 32 , p);
		memcpy(detectSTR,popenData,5);
		if(!strcmp(detectSTR,"DATA:"))
			//strlen(data)-6 is 5 for DATA: and 1 for \n
			memcpy(hwid,popenData+5,(strlen(popenData)-6));
	}

	if(!strcmp(hwid,""))
		return FALSE;
#ifdef DEBUG_MODE
	printf("HWID:%s\n",hwid);
#endif

	if (pclose(p) == -1) 
	{
		fprintf(stderr,"Error!\n");
		return FALSE;
	}
	
	return TRUE;
}
