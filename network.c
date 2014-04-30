/* credits for init_string and callback function to: 
 * http://stackoverflow.com/a/2329792/3095632
 */

#include "header.h"

static void init_string(ResponseString *s) 
{
	s->len = 0;
	s->ptr = malloc(s->len+1);
	if (s->ptr == NULL) 
	{
		fprintf(stderr, "malloc() failed\n");
		exit(EXIT_FAILURE);
	}
	s->ptr[0] = '\0';
}

static size_t curl_response_to_string(void *ptr, size_t size, size_t nmemb, ResponseString *s)
{
	size_t new_len = s->len + size*nmemb;
	s->ptr = realloc(s->ptr, new_len+1);
	if (s->ptr == NULL) 
	{
		fprintf(stderr, "realloc() failed\n");
		exit(EXIT_FAILURE);
	}
	memcpy(s->ptr+s->len, ptr, size*nmemb);
	s->ptr[new_len] = '\0';
	s->len = new_len;

	return size*nmemb;
}

gboolean send_reg_jsonstring_to_server(const char* jsonString, const char* serverName)
{
	gboolean retFlag = FALSE;
	
	CURL *curl;
	CURLcode res;

	char *dataBuffer;
	dataBuffer = (char *) malloc ((strlen(jsonString)+5)*sizeof(char));
	if(dataBuffer == NULL) 
		return FALSE;
	
	memset(dataBuffer,0,sizeof(dataBuffer));
	strcpy(dataBuffer,"data=");
	memcpy(dataBuffer+5,jsonString, strlen(jsonString));

#ifdef DEBUG_MODE	
	printf("dataBuffer = %s\n",dataBuffer);
#endif

	/* get a curl handle */ 
	curl = curl_easy_init();
	if(curl) 
	{
		ResponseString response;
		init_string(&response);
		
		/* First set the URL that is about to receive our POST. This URL can
		just as well be a https:// URL if that is what should receive the
		data. */ 
		curl_easy_setopt(curl, CURLOPT_URL, serverName);
		
		/* Now specify the POST data */ 
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, dataBuffer);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_response_to_string);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		
		/* Perform the request, res will get the return code */ 
		res = curl_easy_perform(curl);
		
		/* Check for errors */ 
		if(res != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			free(response.ptr);
			curl_easy_cleanup(curl);
			return FALSE;
		}
		
#ifdef DEBUG_MODE		
		printf("response in string:%s\n", response.ptr);
		printf("length:%d\n", response.len);
#endif
		
		//~ memcpy(serverResponse, response.ptr, response.len);
		json_object * jobj_response = json_tokener_parse(response.ptr);
		
		json_object * error_object = json_object_object_get(jobj_response, "error");
		if(strcmp(json_object_to_json_string(error_object),"null"))
			return FALSE;
			
		json_object * response_status = json_object_object_get(jobj_response,"result");
		if(!strcmp(json_object_get_string(response_status),"Error"))
			retFlag = FALSE;
		if(!strcmp(json_object_get_string(response_status),"error"))
			retFlag = FALSE;
		if(!strcmp(json_object_get_string(response_status),"Success"))
			retFlag = TRUE;
		if(!strcmp(json_object_get_string(response_status),"success"))
			retFlag = TRUE;
			
		free(response.ptr);
		
		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}

	return retFlag;
}

gboolean send_absen_jsonstring_to_server(const char* ACCNM, const char* HWID, const char* ACCNP, const char* timestamp, const char* serverName)
{
	gboolean returnFlag = FALSE;
	
	const char *accnmPost = "ACCN-M=";
	const char *hwidPost = "&HWID=";
	const char *accnpPost = "&ACCN-P=";
	const char *timestampPost = "&timestamp=";
	CURL *curl;
	CURLcode res;

	char *dataBuffer;
	int total_len = strlen(accnmPost)+strlen(ACCNM)+strlen(hwidPost)+strlen(HWID)+strlen(accnpPost)+strlen(ACCNP)+strlen(timestampPost)+strlen(timestamp);
	
	dataBuffer = (char *) malloc (total_len+1);
	if(dataBuffer == NULL) 
		return FALSE;
	
	int index = 0;
	
	memset(dataBuffer,0,total_len+1);

	//ACCN-M
	memcpy(dataBuffer,accnmPost,strlen(accnmPost));
	index += strlen(accnmPost);
	
	memcpy(dataBuffer+index,ACCNM, strlen(ACCNM));
	index += strlen(ACCNM);
	
	//HWID
	memcpy(dataBuffer+index,hwidPost,strlen(hwidPost));
	index += strlen(hwidPost);
	
	memcpy(dataBuffer+index,HWID, strlen(HWID));
	index += strlen(HWID);
	
	//ACCN-P
	memcpy(dataBuffer+index,accnpPost,strlen(accnpPost));
	index += strlen(accnpPost);
	
	memcpy(dataBuffer+index,ACCNP, strlen(ACCNP));
	index += strlen(ACCNP);
	
	//timestamp
	memcpy(dataBuffer+index,timestampPost,strlen(timestampPost));
	index += strlen(timestampPost);
	
	memcpy(dataBuffer+index,timestamp, strlen(timestamp));
	index += strlen(timestamp);
	
	dataBuffer[index] = '\0';

#ifdef DEBUG_MODE	
	printf("dataBuffer:%s\n",dataBuffer);
#endif

	/* get a curl handle */ 
	curl = curl_easy_init();
	if(curl) 
	{
		ResponseString response;
		init_string(&response);
		
		/* First set the URL that is about to receive our POST. This URL can
		just as well be a https:// URL if that is what should receive the
		data. */ 
		curl_easy_setopt(curl, CURLOPT_URL, serverName);
		
		/* Now specify the POST data */ 
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, dataBuffer);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_response_to_string);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		
		/* Perform the request, res will get the return code */ 
		res = curl_easy_perform(curl);
		
		/* Check for errors */ 
		if(res != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			return FALSE;
		}
		
#ifdef DEBUG_MODE		
		printf("response in string:%s\n", response.ptr);
		printf("response length:%d\n", response.len);
#endif
		
		//memcpy(serverResponse, response.ptr, response.len);
		json_object * jobj_response_root = json_tokener_parse(response.ptr);
		
		json_object * error_object = json_object_object_get(jobj_response_root, "error");
		if(strcmp(json_object_to_json_string(error_object),"null"))
			return FALSE;
		
		json_object * jobj_response_result = json_object_object_get(jobj_response_root, "result");
		if(!strcmp(json_object_get_string(jobj_response_result),"Error"))
			returnFlag =  FALSE;
		if(!strcmp(json_object_get_string(jobj_response_result),"error"))
			returnFlag =  FALSE;
			
		if(!strcmp(json_object_get_string(jobj_response_result),"success"))
			returnFlag = TRUE;
		if(!strcmp(json_object_get_string(jobj_response_result),"Success"))
			returnFlag = TRUE;
		
		free(response.ptr);
		
		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	
	return returnFlag;
}
