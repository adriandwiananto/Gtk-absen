#include "header.h"

static gboolean qr_valid_data(gpointer nothing);

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
	newQRwindow->continue_button = GTK_WIDGET (gtk_builder_get_object (builder, "new_qr_continue_button"));
	newQRwindow->cancel_button = GTK_WIDGET (gtk_builder_get_object (builder, "new_qr_cancel_button"));

	gtk_builder_connect_signals (builder, newQRwindow);
	g_object_unref(G_OBJECT(builder));
	
	return TRUE;
}

static void kill_qr_poll_process()
{
	/*check child process availability, if exists kill it*/
	if(kill(qr_zbar_pid, 0) >= 0)
	{
		if(kill(qr_zbar_pid, SIGTERM) >= 0)
		{
			printf("qr poll process killed with SIGTERM\n");
		}
		else
		{
			kill(qr_zbar_pid, SIGKILL);
			printf("qr poll process killed with SIGKILL\n");
		}
	}
	else printf("qr poll child process does not exists\n");
}

/* parse nfc data from child process */
static gboolean parse_qr_data(GString *nfcdata)
{
	gsize data_len;
	/* subtract 5 from prefix "QR-Code:"
	 * subtract 1 from postfix newline
	 */
	data_len = (nfcdata->len)-9;

	gchar data_only[262];
	memset(data_only,0,262);
	
	memcpy(data_only,(nfcdata->str)+8,data_len);
	
	unsigned char ReceivedData[data_len/2];
	hexstrToBinArr(ReceivedData, data_only, data_len/2);

	print_array_inHex("Received Data:", ReceivedData, data_len/2);
			
	unsigned char payload[data_len/2];
	memcpy(payload,ReceivedData, data_len/2);
	
	return parse_transaction_frame(payload);
}

/* child process watch callback */
static void cb_child_watch( GPid pid, gint status, GString *data )
{
	data = g_string_new(NULL);
	
	gtk_widget_set_sensitive(newQRwindow->continue_button, TRUE);
	gtk_widget_set_sensitive(newQRwindow->cancel_button, TRUE);

	/* Close pid */
    g_spawn_close_pid( pid );
    
    g_string_free(data,TRUE);
}

/* io out watch callback */
static gboolean cb_out_watch( GIOChannel *channel, GIOCondition cond, GString *data )
{
	GIOStatus status;
	
	gchar detect_str[10];
	memset(detect_str,0,10);
	
	data = g_string_new(NULL);

    if( cond == G_IO_HUP )
    {
        g_io_channel_unref( channel );
        return( FALSE );
    }

    status = g_io_channel_read_line_string( channel, data, NULL, NULL );
 
    switch(status)
    {
		case G_IO_STATUS_EOF:
			printf("EOF\n");
			break;
			
		case G_IO_STATUS_NORMAL:
			memcpy(detect_str,data->str,8);
			if(!strcmp(detect_str,"QR-Code:"))
			{
				if(parse_qr_data(data) == TRUE)
				{
					kill_qr_poll_process();
					
					g_idle_add(qr_valid_data, NULL);
				}
			}
			
			break;
	
		case G_IO_STATUS_AGAIN: break;
		case G_IO_STATUS_ERROR:
		default:
			printf("Error stdout from child process\n");
			error_message("Error reading from child process");
			break;
	}
		
    g_string_free(data,TRUE);

    return( TRUE );
}

/* io err watch callback */
static gboolean cb_err_watch( GIOChannel *channel, GIOCondition cond, GString *data )
{
    gchar *string;
    gsize  size;
    
	data = g_string_new(NULL);

    if( cond == G_IO_HUP )
    {
        g_io_channel_unref( channel );
        return( FALSE );
    }

    g_io_channel_read_line( channel, &string, &size, NULL, NULL );
    fprintf(stderr,"%s",string);    
    g_free( string );
    g_string_free(data,TRUE);

    return( TRUE );
}

/* create child process for nfc poll (call other program) */
static void qr_poll_child_process()
{
    GPid        pid;
    gchar      *argv[] = { "/usr/bin/zbarcam", NULL };
    gint        out,
                err;
    GIOChannel *out_ch,
               *err_ch;
    gboolean    ret;

	GString *data;
	data = g_string_new(NULL);

    /* Spawn child process */
    ret = g_spawn_async_with_pipes( NULL, argv, NULL,
                                    G_SPAWN_DO_NOT_REAP_CHILD, NULL,
                                    data, &pid, NULL, &out, &err, NULL );
    if( ! ret )
    {
        g_error( "SPAWN FAILED" );
        return;
    }
    
	qr_zbar_pid = pid;
	
    /* Add watch function to catch termination of the process. This function
     * will clean any remnants of process. */
    g_child_watch_add( pid, (GChildWatchFunc)cb_child_watch, data );

    /* Create channels that will be used to read data from pipes. */
    out_ch = g_io_channel_unix_new( out );
    err_ch = g_io_channel_unix_new( err );

    /* Add watches to channels */
    g_io_add_watch( out_ch, G_IO_IN | G_IO_HUP, (GIOFunc)cb_out_watch, data );
    g_io_add_watch( err_ch, G_IO_IN | G_IO_HUP, (GIOFunc)cb_err_watch, data );
    
    g_string_free(data,TRUE);
}

void on_new_qr_continue_button_clicked()
{
	gtk_widget_set_sensitive(newQRwindow->continue_button, FALSE);
	gtk_widget_set_sensitive(newQRwindow->cancel_button, FALSE);
	qr_poll_child_process();
}

void on_new_qr_cancel_button_clicked()
{
	kill_qr_poll_process();
	
	Bitwise WindowSwitcherFlag;
	f_status_window = FALSE;	//hide all window
	f_main_window = TRUE;		//show password window
	WindowSwitcher(WindowSwitcherFlag);
}

static gboolean qr_valid_data(gpointer nothing)
{
	Bitwise WindowSwitcherFlag;
	f_status_window = FALSE;
	f_sending_window = TRUE;
	f_main_window = TRUE;
	WindowSwitcher(WindowSwitcherFlag);
	return FALSE;
}
