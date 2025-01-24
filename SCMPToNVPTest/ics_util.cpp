
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ics.h"
#include "soapINVPTransactionProcessorProxy.h"
#include "NVPCybersource.h"

#define SCMP_REQUEST_ICS_APPLICATIONS "SCMP_REQUEST_ICS_APPLICATIONS"
#define MAX_KEY_LENGTH 100
#define MAX_VALUE_LENGTH 100

#pragma comment(lib, "NVPClient.lib")
#pragma comment(lib, "BASECLIENT.lib")
#pragma comment(lib, "ics2api.lib")


//const char CYBS_INI_FILE[]   = "../cybs.ini";
const char CYBS_INI_FILE[] = "..\\resources\\cybs.ini";


void printMap(std::map <std::wstring, std::wstring> m) {
    typedef std::map <std::wstring, std::wstring>::const_iterator it_type;
    for (it_type iterator = m.begin(); iterator != m.end(); iterator++) {
#ifdef WIN32
        wprintf(L"%s", iterator->first.c_str());
        wprintf(L"==>");
        wprintf(L"%s", iterator->second.c_str());
        wprintf(L"\n");
#else
        wprintf(L"%S", iterator->first.c_str());
        wprintf(L"==>");
        wprintf(L"%S", iterator->second.c_str());
        wprintf(L"\n");
#endif

    }
}

/*
std::wstring charToWString(const char* charArray) {
    size_t length = mbstowcs_s(nullptr, charArray, 0);
    if (length == static_cast<size_t>(-1)) {
        throw std::runtime_error("Conversion error");
    }
    std::wstring wstr(length, L'\0');
    mbstowcs_s(&wstr[0], charArray, length);
    return wstr;
}
*/

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

ics_msg* convertSimpleOrderResponseToICSResponse(std::map <std::wstring, std::wstring> soResponse) {
    ics_msg* icsResponse;
    icsResponse = ics_init(0);

    /*
    Sample Simple Order response for reference

    purchaseTotals_currency=USD
    ccAuthReply_cardPINlessDebit=Y
    ccAuthReply_authorizationCode=888888
    ccAuthReply_cardRegulated=Y
    ccAuthReply_cardPayroll=Y
    ccAuthReply_cardCommercial=Y
    card_cardType=001
    ccAuthReply_cardIssuerCountry=840
    ccAuthReply_cardHealthcare=Y
    ccAuthReply_cardLevel3Eligible=Y
    decisionEarlyReply_rcode=1
    ccAuthReply_cardSignatureDebit=Y
    reasonCode=100
    ccAuthReply_cardPrepaid=Y
    ccAuthReply_processorResponse=100
    ccAuthReply_paymentNetworkTransactionID=123456789619999
    pos_terminalID=123456
    ccAuthReply_avsCode=X
    decision=ACCEPT
    ccAuthReply_reasonCode=100
    merchantReferenceCode=080
    ccAuthReply_reconciliationID=70877315D5XP6CL7
    ccAuthReply_amount=42.97
    requestID=7376721305846381904602
    ccAuthReply_authorizedDateTime=2025-01-23T22:42:11Z
    requestToken=Axj/7wSTj+SSsB9tQYLaABEg3YOG7dmxaxGtig2hzG6e/IAOSTAKe/IAOSTaQNhkpz8MmkmXoxh9qbwJycfySVgPtqDBbQAA8TJd
    ccAuthReply_affluenceIndicator=Y
    ccAuthReply_avsCodeRaw=I1
    decisionEarlyReply_reasonCode=100
    */

    //TODO: Use a lookup table for the mapping

    ics_fadd(icsResponse, (char*)"currency", wstring_to_char(soResponse.at(L"purchaseTotals_currency")));
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

    //Add more response fields.

    return icsResponse;
}
/**
 * Process the given ICS request by translating it to a Simple Order request and send to the SO API. The SO response is then converted to ICS response object.
 */
ics_msg *processRequest(ics_msg *icsRequest){
    ics_msg * icsResponse = NULL;

    INVPTransactionProcessorProxy proxy = INVPTransactionProcessorProxy();

	soap_mode(proxy.soap, (soap_mode)SOAP_XML_CANONICAL);
	
	soap_set_omode(proxy.soap, SOAP_C_UTFSTRING);
	soap_set_imode(proxy.soap, SOAP_C_UTFSTRING);

	soap_set_omode(proxy.soap, SOAP_C_MBSTRING);
	soap_set_imode(proxy.soap, SOAP_C_MBSTRING);
	
	std::map <std::wstring, std::wstring> request;

    /* Original SO request 
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
    */
   
	request = convertICSRequestToSimpleOrderRequest(icsRequest);
	printf("Simple Order CREDIT CARD AUTHORIZATION REQUEST: \n" );
	printMap (request);
	std::map <std::wstring, std::wstring> resMap;

    CybsMap *cfgMap = cybs_create_map();

    //initialize the configuration
    cybs_load_config(CYBS_INI_FILE, cfgMap);

    // send the simple order transaction to the gateway. Response map is populated with the response.
	int status = runTransaction(&proxy, cfgMap, request, resMap);
    printf("Simple Order Response:\n");
    printMap (resMap);

    //Convert the map response to ICS response object
    icsResponse = convertSimpleOrderResponseToICSResponse(resMap);
  
    return icsResponse;
}


void runAuthTest() {
    ics_msg* icsorder;
    icsorder = ics_init(0);
    ics_fadd(icsorder, (char*)"ics_applications", (char*)"ics_auth");
    ics_fadd(icsorder, (char*)"merchant_id", (char*)"ng_gpn");
    ics_fadd(icsorder, (char*)"customer_firstname", (char*)"John");
    ics_fadd(icsorder, (char*)"customer_lastname", (char*)"Doe");
    ics_fadd(icsorder, (char*)"customer_email", (char*)"nobody@cybersource.com");
    ics_fadd(icsorder, (char*)"customer_phone", (char*)"408-556-9100");
    ics_fadd(icsorder, (char*)"bill_address1", (char*)"1295 Charleston Rd.");
    ics_fadd(icsorder, (char*)"bill_city", (char*)"Mountain View");
    ics_fadd(icsorder, (char*)"bill_state", (char*)"CA");
    ics_fadd(icsorder, (char*)"bill_zip", (char*)"94043-1307");
    ics_fadd(icsorder, (char*)"bill_country", (char*)"US");
    ics_fadd(icsorder, (char*)"customer_cc_number", (char*)"4111111111111111");
    ics_fadd(icsorder, (char*)"customer_cc_expmo", (char*)"12");
    ics_fadd(icsorder, (char*)"customer_cc_expyr", (char*)"2030");
    ics_fadd(icsorder, (char*)"merchant_ref_number", (char*)"12");
    ics_fadd(icsorder, (char*)"currency", (char*)"USD");
    ics_fadd(icsorder, (char*)"offer0", (char*)"offerid:0^amount:4.59");

    ics_msg* icsResponse = processRequest(icsorder);
}

int main() {
    printf("Running auth transaction");
    runAuthTest();
    return 0;
}
	