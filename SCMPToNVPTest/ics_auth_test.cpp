#include <stdio.h>
#include "ics.h"
#include "ics_util.h"

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


int main() {
    printf("Running auth transaction\n");
    runAuthTest();
    return 0;
}