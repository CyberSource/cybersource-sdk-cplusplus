//GENAI=YES

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
#include "soapINVPTransactionProcessorProxy.h"
#include "NVPCybersource.h"
#include "ics.h"
#include "ics_util.h"



void printMap(std::map <std::wstring, std::wstring> m);
std::map<std::wstring, std::wstring> loadPropertiesFile(const std::string& filename);
std::map <std::wstring, std::wstring> convertICSRequestToSimpleOrderRequest(ics_msg* icsRequest);
ics_msg* convertSimpleOrderResponseToICSResponse(std::map <std::wstring, std::wstring> soResponse);
char* wstring_to_char(const std::wstring& wstr);
std::wstring charToWString(const char* charArray);
std::vector<std::wstring> splitWString(const std::wstring& str, wchar_t delimiter);
std::wstring stringToWString(const std::string& str);
std::string wstringToString(const std::wstring& wstr);


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

std::map <std::wstring, std::wstring> convertICSRequestToSimpleOrderRequest(ics_msg* icsRequest) {
    std::map <std::wstring, std::wstring> soRequest;

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

    return soRequest;
}

ics_msg* convertSimpleOrderResponseToICSResponse(std::map <std::wstring, std::wstring> soResponse) {
    ics_msg* icsResponse;
    icsResponse = ics_init(0);

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

