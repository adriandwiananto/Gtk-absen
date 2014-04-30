#include "header.h"

/*
We call init_mainmenu_window() when our program is starting to load 
main menu window with references to Glade file. 
*/
gboolean init_sending_window()
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
	sendingWindow->window = GTK_WIDGET (gtk_builder_get_object (builder, "sending_window"));
	sendingWindow->label = GTK_WIDGET (gtk_builder_get_object (builder, "sending_label"));

	gtk_builder_connect_signals (builder, sendingWindow);
	g_object_unref(G_OBJECT(builder));
	
	return TRUE;
}

gpointer build_and_send_absenData(gpointer nothing)
{
	json_object* absenData_jobj;
	absenData_jobj = create_absen_json();
	
	if(strlen(json_object_to_json_string(absenData_jobj)) <= 3)
	{
		g_idle_add(sending_finish, "Connect reader!");
		return NULL;
	}
	
	json_object * ACCNM = json_object_object_get(absenData_jobj, "ACCN-M");
	json_object * HWID = json_object_object_get(absenData_jobj, "HWID");
	json_object * ACCNP = json_object_object_get(absenData_jobj, "ACCN-P");
	json_object * timestamp = json_object_object_get(absenData_jobj, "timestamp");
	
	printf("ACCN-M:%s\n", json_object_to_json_string(ACCNM));
	printf("HWID:%s\n", json_object_to_json_string(HWID));
	printf("ACCN-P:%s\n", json_object_to_json_string(ACCNP));
	printf("timestamp:%s\n", json_object_to_json_string(timestamp));
	
	if(send_absen_jsonstring_to_server	(json_object_to_json_string(ACCNM), 
										json_object_to_json_string(HWID),
										json_object_to_json_string(ACCNP),
										json_object_to_json_string(timestamp),
										"https://emoney-server.herokuapp.com/presence.json") == FALSE)
	{
		g_idle_add(sending_finish, "Error!");
		return NULL;
	}
	g_idle_add(sending_finish, "Success!");
	return NULL;
}

gboolean sending_finish(gpointer message)
{
	Bitwise WindowSwitcherFlag;
	f_status_window = FALSE;	//hide all window
	f_main_window = TRUE;		//show password window
	WindowSwitcher(WindowSwitcherFlag);

	if(!strcmp((const char*)message, "Success!"))
	{
		notification_message((const gchar*)message);
	}
	else
	{
		error_message((const gchar*) message);
	}
		
	return FALSE;
}
