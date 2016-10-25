// xml.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "XMLCybersource.h"
#include "util.h"
#include "stdsoap2.h" 
#include "soapStub.h"
#include <sstream>
#include <cstring>
#include <string>
#include <map>

#pragma comment(lib, "XMLClient.lib")
#pragma comment(lib, "BASECLIENT.lib")

void handleFault(ITransactionProcessorProxy proxy);
void printRequest(ITransactionProcessorProxy proxy, ns2__RequestMessage *request);
void printResponse(ITransactionProcessorProxy proxy, ns2__ReplyMessage *reply);

const char CYBS_INI_FILE[]   = "../cybs.ini";
const char AUTH_XML_FILE[]   = "../auth.xml";

char* getInputXML (const char* xmlfile) {
	FILE *fp;
	char *src = NULL;
    long  size;

	fp = fopen( xmlfile, "r" );

	if (fp) {
		fseek( fp, 0, SEEK_END );
		size = ftell( fp );
		rewind( fp );
		src = (char *) malloc( size+1 );

		if( src ) {
			fread( src, 1, size, fp );
			*(src+size) = '\0';
		}
	fclose( fp );
	}
	return( src );

}

ns2__RequestMessage authTransaction(soap *ctx) {
    char *c = getInputXML(AUTH_XML_FILE);
	if ( c == NULL ) {
		printf( "The following error occurred before the request could be sent:\n" );
		printf( "  Could not read XML input file [%s].\n", AUTH_XML_FILE );
    }
	ns2__RequestMessage req = ns2__RequestMessage();
	std::istringstream iss;
	iss.str (c);
	ctx->is = &iss;
	soap_read_ns2__RequestMessage(ctx, &req);
	free(c);
    return (req);
}

void handleError( int status, ns2__RequestMessage request, CybsMap *store )
{
	printf( "RunTransaction Status: %d\n", status );

	switch( status ) {
		case CYBS_S_PRE_SEND_ERROR:
			printf( "The following error occurred before the request could be sent: \n" );
			printf( "%s\n", cybs_get(store, CYBS_SK_ERROR_INFO));
		break;
		case CYBS_S_SEND_ERROR:
			printf( "The following error occurred: \n" );
			printf( "%s\n", cybs_get(store, CYBS_SK_ERROR_INFO));
		break;
		default:
			printf( "The following error occurred: \n" );
			printf( "%s\n", cybs_get(store, CYBS_SK_ERROR_INFO));
		break;
  }
}

char *run () {
	char *ret = NULL;
	soap *ctx = soap_new();
	soap_set_omode(ctx, SOAP_C_UTFSTRING);
	soap_set_imode(ctx, SOAP_C_UTFSTRING);
	
	soap_set_omode(ctx, SOAP_C_MBSTRING);
	soap_set_imode(ctx, SOAP_C_MBSTRING);
	
	ns2__ReplyMessage *reply = new ns2__ReplyMessage();
	ITransactionProcessorProxy proxy = ITransactionProcessorProxy ();
	soap_mode(proxy.soap, (soap_mode)SOAP_XML_CANONICAL);
	
	soap_set_omode(proxy.soap, SOAP_C_UTFSTRING);
	soap_set_imode(proxy.soap, SOAP_C_UTFSTRING);

	soap_set_omode(proxy.soap, SOAP_C_MBSTRING);
	soap_set_imode(proxy.soap, SOAP_C_MBSTRING);
	
	ns2__RequestMessage req = authTransaction(ctx);
	CybsMap *configMap = cybs_create_map();


	cybs_load_config (CYBS_INI_FILE, configMap);

	if ( !configMap ) {
		printf( "Error: could not find the config file [%s]\n", CYBS_INI_FILE );
		//return(1);
	}

	printf( "CREDIT CARD AUTHORIZATION REQUEST: \n" );
	printRequest(proxy, &req);

	int  status = cybs_runTransaction(&proxy, &req, &reply, configMap);
	
	switch (status)
	{
		case SOAP_OK:
			printf( "CREDIT CARD AUTHORIZATION REPLY: \n" );
			//printf("decision = %s\n", (reply->decision).c_str());
			//printf("reasonCode = %d\n", (reply->reasonCode).c_str());
			//printf("requestID = %s\n", (reply->requestID).c_str());
			//printf("requestToken = %s\n", (reply->requestToken).c_str());
			//printf("ccAuthReply.reasonCode = %d\n", (reply->ccAuthReply->reasonCode).c_str());
			printResponse(proxy, reply);

		break;

		case SOAP_FAULT:
			handleFault(proxy);
		break;

		case CYBS_S_PRE_SEND_ERROR:
			if (cybs_get(configMap, CYBS_SK_ERROR_INFO)) {
				printf( "%s\n", cybs_get(configMap, CYBS_SK_ERROR_INFO));
			}
		break;

		default:
			if (cybs_get(configMap, CYBS_SK_ERROR_INFO)) {
				printf( "\n%s\n", cybs_get(configMap, CYBS_SK_ERROR_INFO));
			} else {
				soap_print_fault(proxy.soap, stdout);
				printf( "Error code: %d\nPlease consult the gSOAP documentation.\n", status );
			}
	}

	//reply->~ns2__ReplyMessage();
	delete reply;

	proxy.destroy();
	soap_delete(proxy.soap, NULL);
	soap_dealloc(proxy.soap, NULL);
	soap_destroy(proxy.soap);
	soap_end(proxy.soap);
	soap_done(proxy.soap);

	soap_delete(ctx, NULL);
	soap_dealloc(ctx, NULL);
	soap_destroy(ctx);
	soap_end(ctx);
	soap_done(ctx);
	soap_free(ctx);

	cybs_destroy_map(configMap);
	return( ret );
}

void handleFault(ITransactionProcessorProxy proxy)
{
	soap_print_fault(proxy.soap, stdout );
	printf("faultcode = %s\n", proxy.soap->fault->faultcode);
	printf("faultstring = %s\n", proxy.soap_fault_string());
	SOAP_ENV__Detail *detail  = proxy.soap->fault->detail;
	if (detail != NULL) {
		printf("detail = %s\n", detail->__any);
	}
}

void printRequest(ITransactionProcessorProxy proxy, ns2__RequestMessage *request)
{
	soap_mode(proxy.soap, (soap_mode)SOAP_XML_CANONICAL|SOAP_XML_INDENT);
	std::stringstream ss;
	proxy.soap->os = &ss;
	soap_write_ns2__RequestMessage(proxy.soap, request);
	proxy.soap->os = NULL;
	printf ("\n %s\n",(char *)ss.str().c_str());
	soap_mode(proxy.soap, (soap_mode)SOAP_XML_CANONICAL);
}

void printResponse(ITransactionProcessorProxy proxy, ns2__ReplyMessage *reply)
{
	soap_mode(proxy.soap, (soap_mode)SOAP_XML_CANONICAL|SOAP_XML_INDENT);
	std::stringstream ss;
	proxy.soap->os = &ss;
	soap_write_ns2__ReplyMessage(proxy.soap, reply);
	proxy.soap->os = NULL;
	printf ("\n %s\n",(char *)ss.str().c_str());
	soap_mode(proxy.soap, (soap_mode)SOAP_XML_CANONICAL);
}

int main (void)
{
	run();

	return 0;
}

