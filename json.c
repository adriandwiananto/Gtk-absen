#include "header.h"

const char* get_key_inString_from_json_response(json_object* jobj)
{
	json_object* jobj_parse;
	
	jobj_parse = json_object_object_get(jobj, "key");
	
	return json_object_get_string(jobj_parse);
}

json_object* create_registration_json(uintmax_t ACCN, int HWID)
{
	/* Can not use get_ACCN yet. ACCN hasn't been written on config files */
	gchar ACCNstr[32];
	memset(ACCNstr, 0, 32);
	sprintf(ACCNstr,"%ju",ACCN);
	
	/*Creating a json object*/
	json_object * jobj = json_object_new_object();

	/*Creating a json string*/
	//~ json_object *jint64_ACCN = json_object_new_int64(ACCN);
	json_object *jint64_ACCN = json_object_new_string(ACCNstr);

	/*Creating a json string*/
	json_object *jint_HWID = json_object_new_int(HWID);

	/*Form the json object*/
	/*Each of these is like a key value pair*/
	json_object_object_add(jobj,"ACCN", jint64_ACCN);
	json_object_object_add(jobj,"HWID", jint_HWID);
	
	return jobj;
}

//~ json_object* create_log_as_json_object()
json_object* create_absen_json()
{
	json_object * jobj = json_object_new_object();
	
	uintmax_t ACCN;
	gchar ACCNstr[32];
	ACCN = get_ACCN(ACCNstr);
	json_object * jint_ACCN;
	//~ if(get_INT64_from_config(&ACCN, "application.ACCN") == TRUE)
	jint_ACCN = json_object_new_int64(ACCN);
	
	char HWID[16];
	memset(HWID,0,16);
	json_object * jint_HWID;
	if(get_USB_reader_HWID(HWID) == TRUE)
	{
		int HWIDint = strtoimax(HWID,NULL,10);
		jint_HWID = json_object_new_int(HWIDint);
	}
	else
	{
		return json_object_new_object();
	}
	
	json_object_object_add(jobj,"ACCN-M",jint_ACCN);
	printf("json object:%s\n",json_object_to_json_string(jobj));

	json_object_object_add(jobj,"HWID",jint_HWID);
	printf("json object:%s\n",json_object_to_json_string(jobj));

	json_object * jint_ACCNP = json_object_new_int64(lastAbsentData.ACCNlong);
	json_object_object_add(jobj,"ACCN-P",jint_ACCNP);
	printf("json object:%s\n",json_object_to_json_string(jobj));

	json_object * jint_TS = json_object_new_int64(lastAbsentData.TSlong);
	json_object_object_add(jobj,"timestamp",jint_TS);
	printf("json object:%s\n",json_object_to_json_string(jobj));

	return jobj;
}
