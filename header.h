#include <gtk/gtk.h>
//~ #include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
//~ #include <inttypes.h>
//~ #include <glib.h>
//~ #include <time.h>
//~ #include <sys/wait.h>
//~ #include <openssl/aes.h>
//~ #include <openssl/rand.h>
//~ #include <openssl/sha.h>
//~ #include <openssl/bio.h>
//~ #include <openssl/evp.h>
//~ #include <openssl/buffer.h>
#include <curl/curl.h>
#include <json/json.h>

#define DEBUG_MODE

#ifndef _ABSEN_
#define _ABSEN_

/* location of UI XML file relative to path in which program is running */
#define UI_GLADE_FILE "absen_ui.glade"

/* crypto definition */
//~ #define AES_MODE (256)
//~ #define KEY_LEN_BYTE AES_MODE/8 

#define f_main_window				WindowSwitcherFlag.bit0
#define f_new_nfc_window			WindowSwitcherFlag.bit1
#define f_new_qr_window				WindowSwitcherFlag.bit2
#define f_registration_window		WindowSwitcherFlag.bit3
#define f_sending_window			WindowSwitcherFlag.bit4
#define f_status_window 			WindowSwitcherFlag.status

typedef struct
{
	char *ptr;
	size_t len;
}ResponseString;

typedef struct
{
	gboolean msgBegin;
	gboolean msgEnd;
	gboolean chunkFlag;
	gboolean shortRec;
	gboolean IDLen;
	char TNF;
	unsigned char typeLen;
	unsigned char payloadLen;
}NdefHeader;

typedef struct
{
	unsigned char PT;

	unsigned char SESNbyte[2];
	unsigned int SESNint;
	
	unsigned char ACCNbyte[6];
	unsigned long long ACCNlong;

	unsigned char TSbyte[4];
	unsigned long TSlong;
}absenteeData;
	
typedef union
{
	struct 
	{
		unsigned char bit0:1;
		unsigned char bit1:1;
		unsigned char bit2:1;
		unsigned char bit3:1;
		unsigned char bit4:1;
		unsigned char bit5:1;
		unsigned char bit6:1;
		unsigned char bit7:1;
	};
	unsigned char status;
}Bitwise;

typedef struct
{
	GtkWidget *window;
}MainMenuWindow;

typedef struct
{
	GtkWidget *window;
	GtkWidget *SESN_label;
	GtkWidget *continue_button;
	GtkWidget *cancel_button;
}NewAbsentWindow;

typedef struct
{
	GtkWidget *window;
	GtkWidget *ACCN_entry;
	GtkWidget *new_entry;
	GtkWidget *confirm_entry;
}RegistrationWindow;

typedef struct
{
	GtkWidget *window;
	GtkWidget *label;
}SendingWindow;

/*callback function*/
void on_new_qr_continue_button_clicked();
void on_new_qr_cancel_button_clicked();
void on_new_nfc_cancel_button_clicked();
void on_registration_request_button_clicked();
void on_registration_cancel_button_clicked();
void on_registration_ACCN_entry_insert_text(GtkEditable *buffer, gchar *new_text, gint new_text_length, gint *position, gpointer data);

/*window init function*/
gboolean init_mainmenu_window();
gboolean init_registration_window();
gboolean init_newnfc_window();
gboolean init_newqr_window();
gboolean init_sending_window();

/*libconfig function*/
int config_checking();
gboolean create_new_config_file(uintmax_t ACCN, char *HWID);
//~ gboolean get_INT64_from_config(uintmax_t *value, const char *path);
//~ gboolean get_string_from_config(char *value, const char *path);
//~ gboolean write_string_to_config(char *value, const char *path);
//~ gboolean write_int64_to_config(uintmax_t value, const char *path);
uintmax_t get_ACCN(gchar* ACCN_inString);

/*crypto function*/
//~ void passwordhashing(char *hashed, const gchar *password, const gchar *salt);
//~ char *unbase64(unsigned char *input, int length);
//~ char *base64(const unsigned char *input, int length);
//~ gboolean wrap_aes_key(unsigned char *out, unsigned char *wrapper_key, unsigned char *key_to_wrap);
//~ gboolean unwrap_aes_key(unsigned char *out, unsigned char *wrapper_key, unsigned char *key_to_unwrap);
//~ gboolean derive_key(unsigned char *out, const gchar *password, const gchar *salt, unsigned int iteration);
//~ void getTransKey(unsigned char* aes_key, const gchar* password, const gchar* ACCN, gboolean printResult);
//~ gboolean decrypt_transaction_frame(unsigned char* output, unsigned char* input, unsigned char* IV);
//~ gboolean getLogKey(unsigned char* logKey);
//~ void json_log_array_hashing(char *hashed, const char *json_array);
//~ int calc_sha256_of_file (char* path, char output[65]);
//~ gboolean build_receipt_packet(gchar* receipt_ndef_str);
//~ void aes256cbc(unsigned char* output, unsigned char* input, unsigned char* key, unsigned char* IV, const char* mode);

/*spawn function*/
void nfc_poll_child_process(gchar *SESN);
//~ void spawn_nfc_receipt_process(gchar* receipt_ndef);
//~ void qr_zbar_child_process();

/*log function*/
//~ gboolean createDB_and_table();
//~ gboolean write_lastTransaction_log();
//~ gboolean encrypt_lastTransaction_log(unsigned char* logHexInStr, unsigned int logNum);
//~ int read_log_blob(unsigned char *dest, int row);
//~ int logNum();
//~ void convert_DBdata_to_TreeView_Data(unsigned char *DB_BLOB_data, int logLen, unsigned int *lognum, char *timebuffer, uintmax_t *senderACCN, unsigned int*amount);
//~ gboolean update_encrypted_log(char* logToWriteInStr, int Row);
//~ gboolean clear_DB_entry();

/*network function*/
gboolean send_reg_jsonstring_to_server(const char* jsonString, const char* serverName);
gboolean send_absen_jsonstring_to_server(const char* ACCNM, const char* HWID, const char* ACCNP, const char* timestamp, const char* serverName);
//~ gboolean send_log_jsonstring_to_server(gchar* aesKeyString, const char* jsonHeader, const char* jsonLogs, const char* serverName, int* return_balance);

/*json function*/
json_object* create_registration_json(uintmax_t ACCN, int HWID);
json_object* create_absen_json();
//~ const char* get_key_inString_from_json_response(json_object* jobj);
//~ json_object* create_log_as_json_object();

/*other function*/
//~ void read_pwd_entry();
void error_message (const gchar *message);
void notification_message (const gchar *message);
void WindowSwitcher(Bitwise WindowSwitcherFlag);
int random_number_generator(int min_number, int max_number);
//~ void parse_log_file_and_write_to_treeview(int startRow, int endRow);
void hexstrToBinArr(unsigned char* dest, gchar* source, gsize destlength);
gboolean get_USB_reader_HWID (char* hwid);
//~ gboolean create_receipt_from_lastTransactionData();
void print_array_inHex(const char* caption, unsigned char* array, int size);
//~ void create_merch_req_png();
gboolean parse_transaction_frame(unsigned char *payload);
//~ void build_and_send_absenData();
gpointer build_and_send_absenData(gpointer nothing);

/*thread ui update*/
gboolean sending_finish(gpointer message);

#ifndef DECLARE_VARIABLES
#define EXTERN /* nothing */
#else
#define EXTERN extern /* declare variable */
#endif

/*global window variable*/
EXTERN MainMenuWindow *mainmenuwindow;
EXTERN NewAbsentWindow *newNFCwindow;
EXTERN RegistrationWindow *registrationwindow;
EXTERN NewAbsentWindow *newQRwindow;
EXTERN SendingWindow *sendingWindow;

/*global variable*/
EXTERN GPid nfc_poll_pid;
//~ EXTERN GPid nfc_receipt_pid;
EXTERN GPid qr_zbar_pid;
//~ EXTERN char nfc_data[128];
EXTERN absenteeData lastAbsentData;
//~ EXTERN CryptoKey cryptoKey;

#endif
