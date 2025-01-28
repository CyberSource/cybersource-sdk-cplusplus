
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <iterator>
#include <locale>
#include <stdexcept>
#include <codecvt>
#include <fstream>
#include <sstream>
#include "ics.h"
#include "soapINVPTransactionProcessorProxy.h"
#include "NVPCybersource.h"

#define SCMP_REQUEST_ICS_APPLICATIONS "SCMP_REQUEST_ICS_APPLICATIONS"
#define MAX_KEY_LENGTH 100
#define MAX_VALUE_LENGTH 100

//const char CYBS_INI_FILE[]   = "..resources/cybs.ini";
const char CYBS_INI_FILE[] = "..\\resources\\cybs.ini";

void printMap(std::map <std::wstring, std::wstring> m);
std::map<std::wstring, std::wstring> loadPropertiesFile(const std::string& filename);
void runAuthTest();
ics_msg* processRequest(ics_msg* icsRequest);
std::map <std::wstring, std::wstring> convertICSRequestToSimpleOrderRequest(ics_msg* icsRequest);
ics_msg* convertSimpleOrderResponseToICSResponse(std::map <std::wstring, std::wstring> soResponse);
char* wstring_to_char(const std::wstring& wstr);
std::wstring charToWString(const char* charArray);
std::vector<std::wstring> splitWString(const std::wstring& str, wchar_t delimiter);
std::wstring stringToWString(const std::string& str);
std::string wstringToString(const std::wstring& wstr);

int main() {
    printf("Running auth transaction\n");
    runAuthTest();
    return 0;
}

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
void runAuthTest() {
    float amount = 20.00;
    char offer[1024];

    ics_msg* icsorder;
    icsorder = ics_init(0);

    ics_fadd(icsorder, (char*)"ics_applications", (char*)"ics_auth");
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

    //add 1 item
    sprintf(offer, "amount:%.2f^merchant_product_sku:GC1^product_name:Gift Certificate^quantity:1",
        amount);
    ics_fadd(icsorder, (char*)"offer0", offer);

    ics_msg* icsResponse = processRequest(icsorder);
    ics_destroy(icsorder);
    ics_destroy(icsResponse);
}

/**
 * Process the given ICS request by translating it to a Simple Order request and send to the SO API. The SO response is then converted to ICS response object.
 */
ics_msg* processRequest(ics_msg* icsRequest) {
    ics_msg* icsResponse = NULL;
    printf("==== SCMP Request ====\n");
    ics_print(icsRequest);

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
    printf("\n==== Simple Order Request ====\n");
    printMap(request);
    std::map <std::wstring, std::wstring> resMap;

    CybsMap* cfgMap = cybs_create_map();

    //initialize the configuration
    cybs_load_config(CYBS_INI_FILE, cfgMap);

    // send the simple order transaction to the gateway. Response map is populated with the response.
    int status = runTransaction(&proxy, cfgMap, request, resMap);
    printf("\n==== Simple Order Response ====\n");
    printMap(resMap);

    //Convert the map response to ICS response object
    icsResponse = convertSimpleOrderResponseToICSResponse(resMap);

    printf("\n==== SCMP Response ====\n");
    ics_print(icsResponse);


    //free the proxy
    proxy.destroy();
    soap_delete(proxy.soap, NULL);
    soap_dealloc(proxy.soap, NULL);
    soap_destroy(proxy.soap);
    soap_end(proxy.soap);
    soap_done(proxy.soap);

    cybs_destroy_map(cfgMap);
    return icsResponse;
}

std::map <std::wstring, std::wstring> convertICSRequestToSimpleOrderRequest(ics_msg* icsRequest) {
    std::map <std::wstring, std::wstring> soRequest;

    /* hard-coded static request
    soRequest[L"merchantReferenceCode"] =  charToWString(ics_fgetbyname(icsRequest, "merchant_ref_number"));
    soRequest[L"billTo_firstName"] = charToWString(ics_fgetbyname(icsRequest, "customer_firstname"));
    soRequest[L"billTo_lastName"] = charToWString(ics_fgetbyname(icsRequest, "customer_lastname"));
    soRequest[L"billTo_street1"] = charToWString(ics_fgetbyname(icsRequest, "bill_address1"));
    soRequest[L"billTo_city"] = charToWString(ics_fgetbyname(icsRequest, "bill_city"));
    soRequest[L"billTo_state"] = charToWString(ics_fgetbyname(icsRequest, "bill_state"));
    soRequest[L"billTo_postalCode"] = charToWString(ics_fgetbyname(icsRequest, "bill_zip"));
    soRequest[L"billTo_country"] = charToWString(ics_fgetbyname(icsRequest, "bill_country"));
    soRequest[L"billTo_email"] = charToWString(ics_fgetbyname(icsRequest, "customer_email"));
    soRequest[L"billTo_ipAddress"] = charToWString(ics_fgetbyname(icsRequest, "customer_ipaddress"));
    soRequest[L"billTo_phoneNumber"] = charToWString(ics_fgetbyname(icsRequest, "customer_phone"));
    soRequest[L"shipTo_firstName"] = charToWString(ics_fgetbyname(icsRequest, "ship_to_firstname"));
    soRequest[L"shipTo_lastName"] = charToWString(ics_fgetbyname(icsRequest, "ship_to_lastname"));
    soRequest[L"shipTo_street1"] = charToWString(ics_fgetbyname(icsRequest, "ship_to_address1"));
    soRequest[L"shipTo_state"] = charToWString(ics_fgetbyname(icsRequest, "ship_to_state"));
    soRequest[L"shipTo_city"] = charToWString(ics_fgetbyname(icsRequest, "ship_to_city"));
    soRequest[L"shipTo_postalCode"] = charToWString(ics_fgetbyname(icsRequest, "ship_to_zip"));
    soRequest[L"shipTo_country"] = charToWString(ics_fgetbyname(icsRequest, "ship_to_country"));
    soRequest[L"card_accountNumber"] = charToWString(ics_fgetbyname(icsRequest, "customer_cc_number"));
    soRequest[L"card_expirationMonth"] = charToWString(ics_fgetbyname(icsRequest, "customer_cc_expmo"));
    soRequest[L"card_expirationYear"] = charToWString(ics_fgetbyname(icsRequest, "customer_cc_expyr"));
    soRequest[L"purchaseTotals_currency"] = charToWString(ics_fgetbyname(icsRequest, "currency"));
    */

    // loads the request mapping table
    std::map<std::wstring, std::wstring> requestMap = loadPropertiesFile("scmp_so_mapping.properties");

    for (int i = 0; i < ics_fcount(icsRequest); ++i) {
        std::wstring icsRequestKey = charToWString(ics_fname(icsRequest, i));

        //check for the ics_applications request field
        if (icsRequestKey == L"ics_applications") {
            std::map<std::wstring, std::wstring> icsApplicationMap = loadPropertiesFile("ics_applications.properties");
            if (icsRequestKey.find(L',') != std::wstring::npos) {
                //this is a bundle call
                std::vector<std::wstring> tokens = splitWString(icsRequestKey, ',');
                for (const auto& token : tokens) {
                    auto icsApp = icsApplicationMap.find(token);
                    if (icsApp != icsApplicationMap.end()) {
                        soRequest[icsApp->second] = L"true";
                    }
                }
            }
            else {
                auto icsAppSingle = icsApplicationMap.find(charToWString(ics_fget(icsRequest, i)));
                if (icsAppSingle != icsApplicationMap.end()) {
                    soRequest[icsAppSingle->second] = L"true";
                }
            }
        }
        else if (icsRequestKey.find(L"offer") != std::wstring::npos) {
            //handle offer/item fields. 
            // SCMP pattern is "amount:4.59^merchant_product_sku:GC1^product_name:Gift Certificate^product_code:123^quantity:1"
            /*
                    // "amount" = "item_i_unitPrice"
                    // "quantity" "item_i_quantity"
                    // "product_name" = "item_i_productName"
                    // "product_description" = "item_i_productDescription"
                    // "product_code" = "item_i_productCode
                    // "merchant_product_sku" = "item_i_productSKU"
                    // "tax_amount" = "item_i_taxAmount"
            */

            //first get the item number(offer0,offer1,offer2,etc)
            std::wstring offerNumber = icsRequestKey.substr(5);
            //get the offer text value
            std::wstring offerValue = charToWString(ics_fget(icsRequest, i));
            //split using delimeter "^"
            std::vector<std::wstring> offerValueSplit = splitWString(offerValue, '^');

            if (!offerValueSplit.empty()) {
                std::wstring itemKey, itemValue;
                for (const std::wstring token : offerValueSplit) {
                    if (token.find(L':') != std::wstring::npos) {
                        //valid key value pair
                        itemKey = token.substr(0, token.find(L':'));//amount:
                        itemValue = token.substr(token.find(L':') + 1);
                        if (itemKey == L"amount") {
                            soRequest[L"item_" + offerNumber + L"_unitPrice"] = itemValue;
                        }
                        else if (itemKey == L"quantity") {
                            soRequest[L"item_" + offerNumber + L"_quantity"] = itemValue;
                        }
                        else if (itemKey == L"product_name") {
                            soRequest[L"item_" + offerNumber + L"_productName"] = itemValue;
                        }
                        else if (itemKey == L"product_description") {
                            soRequest[L"item_" + offerNumber + L"_productDescription"] = itemValue;
                        }
                        else if (itemKey == L"product_code") {
                            soRequest[L"item_" + offerNumber + L"_productCode"] = itemValue;
                        }
                        else if (itemKey == L"merchant_product_sku") {
                            soRequest[L"item_" + offerNumber + L"_productSKU"] = itemValue;
                        }
                        else if (itemKey == L"tax_amount") {
                            soRequest[L"item_" + offerNumber + L"_taxAmount"] = itemValue;
                        }
                    }
                }
            }

        }
        else {
            //look up this key from our request map to get the Simple Order key equivalent
            auto soRequestKey = requestMap.find(icsRequestKey);
            if (soRequestKey != requestMap.end()) {
                // we have a mapping, send it
                soRequest[soRequestKey->second] = charToWString(ics_fget(icsRequest, i));
            }
        }
    }

    //TODO: parse the item/offer text here
    //soRequest[L"item_0_unitPrice"] = L"4.59";

    //soRequest[L"ccAuthService_run"] = L"true";

    return soRequest;
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
    std::map<std::wstring, std::wstring> responseMap = loadPropertiesFile("so_scmp_response_mapping.properties");

    for (const auto& pair : soResponse) {
        const std::wstring& key = pair.first;
        const std::wstring& value = pair.second;

        if (!value.empty()) {
            // look up the key from the properties map. the value will be the key for our SCMP response object
            auto scmpKey = responseMap.find(key);
            if (scmpKey != responseMap.end()) {
                ics_fadd(icsResponse, wstring_to_char(scmpKey->second), wstring_to_char(value));
            }
        }
    }

    return icsResponse;
}


std::wstring charToWString(const char* charArray) {
    size_t length = std::mbstowcs(nullptr, charArray, 0);
    if (length == static_cast<size_t>(-1)) {
        throw std::runtime_error("Conversion error");
    }
    std::wstring wstr(length, L'\0');
    std::mbstowcs(&wstr[0], charArray, length);
    return wstr;
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

/*
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
*/

std::map<std::wstring, std::wstring> loadPropertiesFile(const std::string& filename) {
    std::map<std::wstring, std::wstring> properties;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        throw std::runtime_error("Could not open properties file");
    }

    while (std::getline(file, line)) {
        // Ignore comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream is_line(line);
        std::string key;
        if (std::getline(is_line, key, '=')) {
            std::string value;
            if (std::getline(is_line, value)) {
                properties[stringToWString(key)] = stringToWString(value);
            }
        }
    }

    file.close();
    return properties;
}

std::vector<std::wstring> splitWString(const std::wstring& str, wchar_t delimiter) {
    std::vector<std::wstring> result;
    std::wstringstream wss(str);
    std::wstring token;

    while (std::getline(wss, token, delimiter)) {
        result.push_back(token);
    }

    return result;
}

std::wstring stringToWString(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}
std::string wstringToString(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

