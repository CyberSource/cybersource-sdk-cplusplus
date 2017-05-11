// NVPTest.cpp : Defines the entry po int for the console application.
//

#ifdef WIN32
  #include "stdafx.h"
#else
  #include "ctype.h"
#endif
#include "NVPCybersource.h"
#include "util.h"
//#include <thread>
#include <map>

#pragma comment(lib, "NVPClient.lib")
#pragma comment(lib, "BASECLIENT.lib")

const char CYBS_INI_FILE[]   = "../cybs.ini";


void printContent(CybsMap *map);
std::wstring runAuth(CybsMap *cfgMap);
void runCapture( CybsMap* config, std::wstring authRequestID );
void handleFault(INVPTransactionProcessorProxy proxy);
void printMap (std::map <std::wstring, std::wstring> m);

void handleError( int status, CybsMap *store )
{
	wprintf( L"RunTransaction Status: %d\n", status );
	switch( status ) {
		case CYBS_S_PRE_SEND_ERROR:
			wprintf( L"The following error occurred before the request could be sent:\n" );
			wprintf( L"%s\n", cybs_get(store, CYBS_SK_ERROR_INFO));
		break;
		case CYBS_S_SEND_ERROR:
			wprintf( L"The following error occurred:\n" );
			wprintf( L"%s\n", cybs_get(store, CYBS_SK_ERROR_INFO));
		break;
		default:
			wprintf( L"The following error occurred:\n" );
			wprintf( L"%s\n", cybs_get(store, CYBS_SK_ERROR_INFO));
		break;
	}
}

int main(void)
{
	//Sleep(10000);
	CybsMap *cfgMap = cybs_create_map();
	//char *requestID;
	std::wstring requestID;
	cybs_load_config(CYBS_INI_FILE, cfgMap);

	#ifdef WIN32
		printf( "\nCONFIGURATION (%s): \n", CYBS_INI_FILE);
	#else
		wprintf( L"\nCONFIGURATION (%s): \n", CYBS_INI_FILE);
	#endif

	if ( !cfgMap ) {
		printf( "Error: could not find the config file [%s]\n", CYBS_INI_FILE );
		return(1);
	}

	printContent(cfgMap);
	//int i = 0;
	//while (i<2) {
	requestID = runAuth(cfgMap);
	//i++;

	if ( !requestID.empty() ) {
		runCapture( cfgMap, requestID );
		//free( requestID );
	}
	//}

	cybs_destroy_map(cfgMap);
	return (0);
}

std::wstring runAuth(CybsMap *cfgMap) {
	//const char *decision, *requestID;
	std::wstring decision, requestID;
	char *ret = NULL;
	INVPTransactionProcessorProxy proxy = INVPTransactionProcessorProxy ();

	soap_mode(proxy.soap, (soap_mode)SOAP_XML_CANONICAL);
	
	soap_set_omode(proxy.soap, SOAP_C_UTFSTRING);
	soap_set_imode(proxy.soap, SOAP_C_UTFSTRING);

	soap_set_omode(proxy.soap, SOAP_C_MBSTRING);
	soap_set_imode(proxy.soap, SOAP_C_MBSTRING);
	
	std::map <std::wstring, std::wstring> request;

	request[L"merchantReferenceCode"] = L"your_merchant_reference_code";
	request[L"billTo_firstName"] = L"AJŠAß";
	request[L"billTo_lastName"] = L"Doe";
	request[L"billTo_street1"] = L"1295 Charleston Road";
	request[L"billTo_city"] = L"Mountain View";
	request[L"billTo_state"] = L"CA";
	request[L"billTo_postalCode"] = L"94043";
	request[L"billTo_country"] = L"US";
	request[L"billTo_email"] = L"nobody@cybersource.com";
	request[L"billTo_ipAddress"] = L"10.7.7.7";
	request[L"billTo_phoneNumber"] = L"650-965-6000";
	request[L"shipTo_firstName"] = L"Jane";
	request[L"shipTo_lastName"] = L"Jane";
	request[L"shipTo_street1"] = L"123 toile délavée noir";
	request[L"shipTo_state"] = L"CA";
	request[L"shipTo_city"] = L"CA";
	request[L"shipTo_postalCode"] = L"94401";
	request[L"shipTo_country"] = L"US";
	request[L"card_accountNumber"] = L"4111111111111111";
	request[L"card_expirationMonth"] = L"12";
	request[L"card_expirationYear"] = L"2020";
	request[L"purchaseTotals_currency"] = L"USD";
	request[L"item_0_unitPrice"] = L"12.34";
	request[L"ccAuthService_run"] = L"true";

	wprintf( L"CREDIT CARD AUTHORIZATION REQUEST: \n" );
	printMap (request);
	std::map <std::wstring, std::wstring> resMap;

	int status = runTransaction(&proxy, cfgMap, request, resMap);

	switch (status)
	{
		case SOAP_OK:
			wprintf( L"\nCREDIT CARD AUTHORIZATION REPLY: \n" );
			printMap(resMap);
			decision = resMap.find(L"decision")->second;
			if( wcscmp (decision.c_str(), L"ACCEPT") == 0 ) {
				requestID = resMap.find(L"requestID")->second;
			}
		break;

		case SOAP_FAULT:
			handleFault(proxy);
		break;
		
		case CYBS_S_PRE_SEND_ERROR:
			if (cybs_get(cfgMap, CYBS_SK_ERROR_INFO)) {
				#ifdef WIN32
					printf( "%s\n", cybs_get(cfgMap, CYBS_SK_ERROR_INFO));
				#else
					wprintf( L"%s\n", cybs_get(cfgMap, CYBS_SK_ERROR_INFO));
				#endif
			}
		break;

		default:
			if (cybs_get(cfgMap, CYBS_SK_ERROR_INFO)) {
				#ifdef WIN32
					printf( "%s\n", cybs_get(cfgMap, CYBS_SK_ERROR_INFO));
				#else
					wprintf( L"%s\n", cybs_get(cfgMap, CYBS_SK_ERROR_INFO));
				#endif
			}
			#ifdef WIN32
				printf("\nfaultstring = %s\n", proxy.soap_fault_string());
			#else
				wprintf(L"\nfaultstring = %s\n", proxy.soap_fault_string());
			#endif
	}

	proxy.destroy();
	soap_delete(proxy.soap, NULL);
	soap_dealloc(proxy.soap, NULL);
	soap_destroy(proxy.soap);
	soap_end(proxy.soap);
	soap_done(proxy.soap);

	return( requestID );
}

void runCapture( CybsMap* cfgMap, std::wstring authRequestID ) {
	INVPTransactionProcessorProxy proxy = INVPTransactionProcessorProxy ();
	soap_mode(proxy.soap, (soap_mode)SOAP_XML_CANONICAL);

	std::map <std::wstring, std::wstring> requestMap;
	std::map <std::wstring, std::wstring> resMap;

	requestMap[L"ccCaptureService_run"] = L"true";
	requestMap[L"merchantReferenceCode"] = L"your_merchant_reference_code";
	requestMap[L"ccCaptureService_authRequestID"] = authRequestID;
	requestMap[L"purchaseTotals_currency"] = L"USD";
	requestMap[L"item_0_unitPrice"] = L"12.34";

	wprintf( L"\nFOLLOW-ON CAPTURE REQUEST: \n" );
	printMap(requestMap);
	int status = runTransaction(&proxy, cfgMap, requestMap, resMap);
	
	switch (status)
	{
		case SOAP_OK:
			wprintf( L"\nFOLLOW-ON CAPTURE REPLY: \n" );
			//printContent(responseMap);
			printMap(resMap);
		break;
		
		case SOAP_FAULT:
			handleFault(proxy);
		break;
		
		case CYBS_S_PRE_SEND_ERROR:
			if (cybs_get(cfgMap, CYBS_SK_ERROR_INFO)) {
				#ifdef WIN32
					printf( "%s\n", cybs_get(cfgMap, CYBS_SK_ERROR_INFO));
				#else
					wprintf( L"%s\n", cybs_get(cfgMap, CYBS_SK_ERROR_INFO));
				#endif

			}
		break;

		default:
			if (cybs_get(cfgMap, CYBS_SK_ERROR_INFO)) {
				#ifdef WIN32
					printf( "%s\n", cybs_get(cfgMap, CYBS_SK_ERROR_INFO));
				#else
					wprintf( L"%s\n", cybs_get(cfgMap, CYBS_SK_ERROR_INFO));
				#endif
			}
			printf("faultstring = %s\n", proxy.soap_fault_string());
	}

	proxy.destroy();
	soap_delete(proxy.soap, NULL);
	soap_dealloc(proxy.soap, NULL);
	soap_destroy(proxy.soap);
	soap_end(proxy.soap);
	soap_done(proxy.soap);
}

void handleFault(INVPTransactionProcessorProxy proxy)
{
	#ifdef WIN32
		printf("faultcode = %s\n", proxy.soap->fault->faultcode);
		printf("faultstring = %s\n", proxy.soap_fault_string());
	#else
		wprintf(L"faultcode = %s\n", proxy.soap->fault->faultcode);
		wprintf(L"faultstring = %s\n", proxy.soap_fault_string());
	#endif

	SOAP_ENV__Detail *detail  = proxy.soap->fault->detail;
	if (detail != NULL) {
		#ifdef WIN32
			printf("detail = %s\n", detail->__any);
		#else
			wprintf(L"detail = %s\n", detail->__any);
		#endif
		
	}
}

void printMap (std::map <std::wstring, std::wstring> m) {
	typedef std::map <std::wstring, std::wstring>::const_iterator it_type;
	for(it_type iterator = m.begin(); iterator != m.end(); iterator++) {
		#ifdef WIN32
			wprintf (L"%s", iterator->first.c_str());
			wprintf (L"==>");
			wprintf (L"%s",iterator->second.c_str());
			wprintf (L"\n");
		#else
			wprintf (L"%S", iterator->first.c_str());
                        wprintf (L"==>");
                        wprintf (L"%S",iterator->second.c_str());
                        wprintf (L"\n");
		#endif

	}
}

void printContent(CybsMap *map) {
	int i = 0;
	while (i < map->length) {
		CybsTable table = map->pairs[i];
		#ifdef WIN32
			printf(" %s ==> %s\n", table.key, table.value);
		#else
			wprintf(L" %s ==> %s\n", table.key, table.value);
		#endif
		i = i +1;
	}
	wprintf(L"\n");
}

