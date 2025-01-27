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
#include "ics.h"

#pragma comment(lib, "NVPClient.lib")
#pragma comment(lib, "BASECLIENT.lib")

const char CYBS_INI_FILE[]   = "../cybs.ini";


void printContent(CybsMap *map);
std::wstring runAuth(CybsMap *cfgMap);
void runCapture( CybsMap* config, std::wstring authRequestID );
void handleFault(INVPTransactionProcessorProxy proxy);
void printMap (std::map <std::wstring, std::wstring> m);
void runAuthWithICSRequest(CybsMap *cfgMap);
ics_msg *processRequest(ics_msg *icsRequest);
std::map <std::wstring, std::wstring> convertICSRequestToSimpleOrderRequest(ics_msg* icsRequest);
ics_msg* convertSimpleOrderResponseToICSResponse(std::map <std::wstring, std::wstring> soResponse);
char* wstring_to_char(const std::wstring& wstr);
std::wstring charToWString(const char* char_array);

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
	//requestID = runAuth(cfgMap);
	runAuthWithICSRequest(cfgMap);
	//i++;

	/*
	if ( !requestID.empty() ) {
		runCapture( cfgMap, requestID );
		//free( requestID );
	}
	//}
	*/

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
	request[L"card_accountNumber"] = L"xxxxxxxxxxxxxxxx";
	request[L"card_expirationMonth"] = L"12";
	request[L"card_expirationYear"] = L"2025";
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

void runAuthWithICSRequest(CybsMap *cfgMap) {
	//Compose the SCMP request object
	ics_msg *icsorder;
    icsorder = ics_init(0);
    ics_fadd(icsorder, "ics_applications", "ics_auth");
    ics_fadd(icsorder, "merchant_id", "visa_acceptance_sf_bill_pmt");
    ics_fadd(icsorder, "customer_firstname", "John");
    ics_fadd(icsorder, "customer_lastname", "Doe");
    ics_fadd(icsorder, "customer_email", "nobody@cybersource.com");
    ics_fadd(icsorder, "customer_phone", "408-556-9100");
    ics_fadd(icsorder, "bill_address1", "1295 Charleston Rd.");
    ics_fadd(icsorder, "bill_city", "Mountain View");
    ics_fadd(icsorder, "bill_state", "CA");
    ics_fadd(icsorder, "bill_zip", "94043-1307");
    ics_fadd(icsorder, "bill_country", "US");
    ics_fadd(icsorder, "customer_cc_number", "4111111111111111");
    ics_fadd(icsorder, "customer_cc_expmo", "12");
    ics_fadd(icsorder, "customer_cc_expyr", "2030");
    ics_fadd(icsorder, "merchant_ref_number", "12");
    ics_fadd(icsorder, "currency", "USD");
    ics_fadd(icsorder, "offer0", "offerid:0^amount:4.59");

	ics_msg *icsResponse = processRequest(icsorder, cfgMap);
}
ics_msg *processRequest(ics_msg *icsRequest, CybsMap *cfgMap){
    ics_msg *icsResponse;

    INVPTransactionProcessorProxy proxy = INVPTransactionProcessorProxy();

	soap_mode(proxy.soap, (soap_mode)SOAP_XML_CANONICAL);
	
	soap_set_omode(proxy.soap, SOAP_C_UTFSTRING);
	soap_set_imode(proxy.soap, SOAP_C_UTFSTRING);

	soap_set_omode(proxy.soap, SOAP_C_MBSTRING);
	soap_set_imode(proxy.soap, SOAP_C_MBSTRING);
	
	std::map <std::wstring, std::wstring> soRequest;
	soRequest = convertICSRequestToSimpleOrderRequest(icsRequest);
	// display ICS request
	wprintf("\nSCMP REQUEST: \n" );
	ics_print(icsRequest);
	wprintf("\nSIMPLE ORDER REQUEST: \n" );
	printMap (soRequest);
	std::map <std::wstring, std::wstring> resMap;

	// send the simple order transaction to the gateway. Response map is populated with the response.
	int status = runTransaction(&proxy, cfgMap, soRequest, resMap);
    wprintf("\nSIMPLE ORDER RESPONSE:\n");
    printMap (resMap);

    //Convert the map response to ICS response object
    icsResponse = convertSimpleOrderResponseToICSResponse(resMap);

	// display the ICS response
	wprintf("\nSCMP RESPONSE: \n" );
	ics_print(icsResponse);

	//clean up the proxy object
	proxy.destroy();
	soap_delete(proxy.soap, NULL);
	soap_dealloc(proxy.soap, NULL);
	soap_destroy(proxy.soap);
	soap_end(proxy.soap);
	soap_done(proxy.soap);
  
    return iscResponse;
}

std::map <std::wstring, std::wstring> convertICSRequestToSimpleOrderRequest(ics_msg* icsRequest) {
    std::map <std::wstring, std::wstring> soRequest;

    soRequest[L"merchantReferenceCode"] = charToWString(ics_fgetbyname(icsRequest, (char*)"merchant_ref_number"));
    soRequest[L"billTo_firstName"] = charToWString(ics_fgetbyname(icsRequest, (char*)"customer_firstname"));
    soRequest[L"billTo_lastName"] = charToWString(ics_fgetbyname(icsRequest, (char*)"customer_lastname"));
    soRequest[L"billTo_street1"] = charToWString(ics_fgetbyname(icsRequest, (char*)"bill_address1"));
    soRequest[L"billTo_city"] = charToWString(ics_fgetbyname(icsRequest, (char*)"bill_city"));
    soRequest[L"billTo_state"] = charToWString(ics_fgetbyname(icsRequest, (char*)"bill_state"));
    soRequest[L"billTo_postalCode"] = charToWString(ics_fgetbyname(icsRequest, (char*)"bill_zip"));
    soRequest[L"billTo_country"] = charToWString(ics_fgetbyname(icsRequest, (char*)"bill_country"));
    soRequest[L"billTo_email"] = charToWString(ics_fgetbyname(icsRequest, (char*)"customer_email"));
    soRequest[L"billTo_ipAddress"] = charToWString(ics_fgetbyname(icsRequest, (char*)"customer_ipaddress"));
    soRequest[L"billTo_phoneNumber"] = charToWString(ics_fgetbyname(icsRequest, (char*)"customer_phone"));
    soRequest[L"shipTo_firstName"] = charToWString(ics_fgetbyname(icsRequest, (char*)"ship_to_firstname"));
    soRequest[L"shipTo_lastName"] = charToWString(ics_fgetbyname(icsRequest, (char*)"ship_to_lastname"));
    soRequest[L"shipTo_street1"] = charToWString(ics_fgetbyname(icsRequest, (char*)"ship_to_address1"));
    soRequest[L"shipTo_state"] = charToWString(ics_fgetbyname(icsRequest, (char*)"ship_to_state"));
    soRequest[L"shipTo_city"] = charToWString(ics_fgetbyname(icsRequest, (char*)"ship_to_city"));
    soRequest[L"shipTo_postalCode"] = charToWString(ics_fgetbyname(icsRequest, (char*)"ship_to_zip"));
    soRequest[L"shipTo_country"] = charToWString(ics_fgetbyname(icsRequest, (char*)"ship_to_country"));
    soRequest[L"card_accountNumber"] = charToWString(ics_fgetbyname(icsRequest, (char*)"customer_cc_number"));
    soRequest[L"card_expirationMonth"] = charToWString(ics_fgetbyname(icsRequest, (char*)"customer_cc_expmo"));
    soRequest[L"card_expirationYear"] = charToWString(ics_fgetbyname(icsRequest, (char*)"customer_cc_expyr"));
    soRequest[L"purchaseTotals_currency"] = charToWString(ics_fgetbyname(icsRequest, (char*)"currency"));

    //TODO: parse the item/offer text here
    soRequest[L"item_0_unitPrice"] = charToWString(ics_fgetbyname(icsRequest, (char*)"customer_firstname"));

    //TODO: expand to support other ics_applications like capture, etc and also bundle calls like auth+capture.
    soRequest[L"ccAuthService_run"] = L"true";

    return soRequest;
}
ics_msg* convertSimpleOrderResponseToICSResponse(std::map <std::wstring, std::wstring> soResponse) {
    ics_msg* icsResponse;
    icsResponse = ics_init(0);
	ics_fadd(icsResponse, (char*)"currency", wstring_to_char(soResponse(L"purchaseTotals_currency")));
    ics_fadd(icsResponse, (char*)"auth_card_pinless_debit", wstring_to_char(soResponse[L"ccAuthReply_cardPINlessDebit"]));
    ics_fadd(icsResponse, (char*)"auth_auth_code", wstring_to_char(soResponse[L"ccAuthReply_authorizationCode"]));
    ics_fadd(icsResponse, (char*)"auth_card_regulated", wstring_to_char(soResponse[L"ccAuthReply_cardRegulated"]));
    ics_fadd(icsResponse, (char*)"auth_card_payroll", wstring_to_char(soResponse[L"ccAuthReply_cardPayroll"]));
    ics_fadd(icsResponse, (char*)"auth_card_commercial", wstring_to_char(soResponse[L"ccAuthReply_cardCommercial"]));
    ics_fadd(icsResponse, (char*)"card_type", wstring_to_char(soResponse[L"card_cardType"]));
    ics_fadd(icsResponse, (char*)"auth_card_issuer_country", wstring_to_char(soResponse[L"ccAuthReply_cardIssuerCountry"]));
    ics_fadd(icsResponse, (char*)"auth_card_healthcare", wstring_to_char(soResponse[L"ccAuthReply_cardHealthcare"]));
    ics_fadd(icsResponse, (char*)"auth_card_level_3_eligible", wstring_to_char(soResponse[L"ccAuthReply_cardLevel3Eligible"]));
    ics_fadd(icsResponse, (char*)"ics_rcode", wstring_to_char(soResponse[L"decisionEarlyReply_rcode"]));
    ics_fadd(icsResponse, (char*)"auth_card_signature_debit", wstring_to_char(soResponse[L"ccAuthReply_cardSignatureDebit"]));
    ics_fadd(icsResponse, (char*)"auth_card_prepaid", wstring_to_char(soResponse[L"ccAuthReply_cardPrepaid"]));
    ics_fadd(icsResponse, (char*)"auth_auth_response", wstring_to_char(soResponse[L"ccAuthReply_processorResponse"]));
    ics_fadd(icsResponse, (char*)"auth_payment_network_transaction_id", wstring_to_char(soResponse[L"ccAuthReply_paymentNetworkTransactionID"]));
    ics_fadd(icsResponse, (char*)"terminal_id", wstring_to_char(soResponse[L"pos_terminalID"]));
    ics_fadd(icsResponse, (char*)"auth_auth_avs", wstring_to_char(soResponse[L"ccAuthReply_avsCode"]));
    ics_fadd(icsResponse, (char*)"auth.reason_code", wstring_to_char(soResponse[L"ccAuthReply_reasonCode"]));
    ics_fadd(icsResponse, (char*)"merchant_ref_number", wstring_to_char(soResponse[L"merchantReferenceCode"]));
    ics_fadd(icsResponse, (char*)"auth_trans_ref_no", wstring_to_char(soResponse[L"ccAuthReply_reconciliationID"]));
    ics_fadd(icsResponse, (char*)"auth_auth_amount", wstring_to_char(soResponse[L"ccAuthReply_amount"]));
    ics_fadd(icsResponse, (char*)"request_id", wstring_to_char(soResponse[L"requestID"]));
    ics_fadd(icsResponse, (char*)"auth_auth_time", wstring_to_char(soResponse[L"ccAuthReply_authorizedDateTime"]));
    ics_fadd(icsResponse, (char*)"request_token", wstring_to_char(soResponse[L"requestToken"]));
    ics_fadd(icsResponse, (char*)"auth_affluence_indicator", wstring_to_char(soResponse[L"ccAuthReply_affluenceIndicator"]));
    ics_fadd(icsResponse, (char*)"auth_avs_raw", wstring_to_char(soResponse[L"ccAuthReply_avsCodeRaw"]));
    ics_fadd(icsResponse, (char*)"ics_decision_reason_code", wstring_to_char(soResponse[L"decisionEarlyReply_reasonCode"]));
}

char* wstring_to_char(const std::wstring& wstr) {
    // Calculate the size needed for the char array
    size_t size_needed = wcstombs(nullptr, wstr.c_str(), 0) + 1;
    if (size_needed == (size_t)-1) {
        throw std::runtime_error("Conversion error");
    }

    // Allocate memory for the char array
    char* char_array = new char[size_needed];

    // Perform the conversion
    wcstombs(char_array, wstr.c_str(), size_needed);

    return char_array;
}
std::wstring charToWString(const char* char_array) {
    if (char_array == nullptr) {
        throw std::invalid_argument("Null pointer received");
    }

    // Calculate the size needed for the wide string
    size_t size_needed = mbstowcs(nullptr, char_array, 0) + 1;
    if (size_needed == (size_t)-1) {
        throw std::runtime_error("Conversion error");
    }

    // Allocate memory for the wide string
    std::wstring wstr(size_needed, L'\0');

    // Perform the conversion
    mbstowcs(&wstr[0], char_array, size_needed);

    // Remove the null terminator added by mbstowcs
    wstr.resize(size_needed - 1);

    return wstr;
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

