#include "header.h"
#include <dirent.h>

static pid_t proc_find(const char* name);

/*
We call init_mainmenu_window() when our program is starting to load 
main menu window with references to Glade file. 
*/
gboolean init_mainmenu_window()
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
	mainmenuwindow->window = GTK_WIDGET (gtk_builder_get_object (builder, "absen_main_window"));

	gtk_builder_connect_signals (builder, mainmenuwindow);
	g_object_unref(G_OBJECT(builder));
	
	return TRUE;
}

/* callback for New NFC button in main menu window */
void on_absen_main_nfc_button_clicked()
{
	pid_t pid = proc_find("./picc_emulation_write");
	if(pid == -1)
	{
		/*open new trans window*/
		Bitwise WindowSwitcherFlag;
		f_status_window = FALSE;	//hide all window
		f_new_nfc_window = TRUE;	//show new NFC window
		f_main_window = TRUE;		//show password window
		WindowSwitcher(WindowSwitcherFlag);
	}
	else
	{
		printf("unfinished process pid: %d\n",pid);
		kill(pid, SIGTERM);
		notification_message("Please wait a moment before \nstarting new transaction");
	}
}

/* callback for New QR button in main menu window */
void on_absen_main_qr_button_clicked()
{
	Bitwise WindowSwitcherFlag;
	f_status_window = FALSE;	//hide all window
	f_new_qr_window = TRUE;		//show new NFC window
	f_main_window = TRUE;		//show password window
	WindowSwitcher(WindowSwitcherFlag);
}

static pid_t proc_find(const char* name) 
{
    DIR* dir;
    struct dirent* ent;
    char* endptr;
    char buf[512];

    if (!(dir = opendir("/proc"))) {
        perror("can't open /proc");
        return -1;
    }

    while((ent = readdir(dir)) != NULL) {
        /* if endptr is not a null character, the directory is not
         * entirely numeric, so ignore it */
        long lpid = strtol(ent->d_name, &endptr, 10);
        if (*endptr != '\0') {
            continue;
        }

        /* try to open the cmdline file */
        snprintf(buf, sizeof(buf), "/proc/%ld/cmdline", lpid);
        FILE* fp = fopen(buf, "r");

        if (fp) {
            if (fgets(buf, sizeof(buf), fp) != NULL) {
                /* check the first token in the file, the program name */
                char* first = strtok(buf, " ");
                if (!strcmp(first, name)) {
                    fclose(fp);
                    closedir(dir);
                    return (pid_t)lpid;
                }
            }
            fclose(fp);
        }

    }

    closedir(dir);
    return -1;
}
