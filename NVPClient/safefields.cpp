#include "safefields.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

/*#define _DEBUG 
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>*/

SafeFields gSafeFields;

static const char UNDERSCORE = '_';
static const char REQUEST_MESSAGE[] = "requestMessage";
static const char REPLY_MESSAGE[] = "replyMessage";

SafeFields::SafeFields() {
	m_pMap = cybs_create_map();
	if (!m_pMap) return;

	cybs_add( m_pMap, "item", "unitPrice quantity productCode productName productSKU productRisk taxAmount cityOverrideAmount cityOverrideRate countyOverrideAmount countyOverrideRate districtOverrideAmount districtOverrideRate stateOverrideAmount stateOverrideRate countryOverrideAmount countryOverrideRate orderAcceptanceCity orderAcceptanceCounty orderAcceptanceCountry orderAcceptanceState orderAcceptancePostalCode orderOriginCity orderOriginCounty orderOeriginCountry orderOriginState orderOriginPostalCode shipFromCity shipFromCounty shipFromCountry shipFromState shipFromPostalCode export noExport nationalTax vatRate sellerRegistration buyerRegistration middlemanRegistration pointOfTitleTransfer giftCategory timeCategory hostHedge timeHedge velocityHedge unitOfMeasure taxRate totalAmount discountAmount discountRate commodityCode grossNetIndicator taxTypeApplied discountIndicator alternateTaxID" );

	cybs_add( m_pMap, "ccAuthService", "run cavv commerceIndicator eciRaw xid reconcilationID avsLevel fxQuoteID returnAuthRecord authType verbalAuthCode billPayment" );

	cybs_add( m_pMap, "ccCaptureService", "run authType verbalAuthCode authRequestID transactionToken reconciliationID partialPaymentID purchasingLevel industryDataType" );

	cybs_add( m_pMap, "ccCreditService", "run captureRequestID transactionToken reconciliationID partialPaymentID purchasingLevel industryDataType commerceIndicator billPayment" );

	cybs_add( m_pMap, "ccAuthReversalService", "run authRequestID transactionToken" );

	cybs_add( m_pMap, "ecDebitService", "run paymentMode referenceNumber settlementMethod transactionToken verificationLevel partialPaymentID commerceIndicator" );

	cybs_add( m_pMap, "ecCreditService", "run referenceNumber settlementMethod transactionToken debitRequestID partialPaymentID commerceIndicator" );

	cybs_add( m_pMap, "payerAuthEnrollService", "run httpAccept httpUserAgent merchantName merchantURL purchaseDescription purchaseTime countryCode acquirerBin merchantID" );

	cybs_add( m_pMap, "payerAuthValidateService", "run signedPARes" );

	cybs_add( m_pMap, "taxService", "run nexus noNexus orderAcceptanceCity orderAcceptanceCounty orderAcceptanceCountry orderAcceptanceState orderAcceptancePostalCode orderOriginCity orderOriginCounty orderOriginCountry orderOriginState orderOriginPostalCode sellerRegistration buyerRegistration middlemanRegistration pointOfTitleTransfer" );

	cybs_add( m_pMap, "afsService", "run avsCode cvCode disableAVSScoring" );

	cybs_add( m_pMap, "davService", "run" );

	cybs_add( m_pMap, "exportService", "run addressOperator addressWeight companyWeight nameWeight" );

	cybs_add( m_pMap, "fxRatesService", "run" );

	cybs_add( m_pMap, "bankTransferService", "run" );

	cybs_add( m_pMap, "bankTransferRefundService", "run bankTransferRequestID reconciliationID" );

	cybs_add( m_pMap, "directDebitService", "run dateCollect directDebitText authorizationID transactionType directDebitType validateRequestID" );

	cybs_add( m_pMap, "directDebitRefundService", "run directDebitRequestID reconciliationID" );

	cybs_add( m_pMap, "directDebitValidateService", "run directDebitValidateText" );

	cybs_add( m_pMap, "paySubscriptionCreateService", "run paymentRequestID disableAutoAuth" );

	cybs_add( m_pMap, "paySubscriptionUpdateService", "run" );

	cybs_add( m_pMap, "paySubscriptionEventUpdateService", "run action" );

	cybs_add( m_pMap, "paySubscriptionRetrieveService", "run" );

	cybs_add( m_pMap, "payPalPaymentService", "run cancelURL successURL reconciliationID" );

	cybs_add( m_pMap, "payPalCreditService", "run payPalPaymentRequestID reconciliationID" );

	cybs_add( m_pMap, "voidService", "run voidRequestID" );

	cybs_add( m_pMap, "pinlessDebitService", "run reconciliationID commerceIndicator" );

	cybs_add( m_pMap, "pinlessDebitValidateService", "run" );

	cybs_add( m_pMap, "payPalButtonCreateService", "run buttonType reconciliationID" );

	cybs_add( m_pMap, "payPalPreapprovedPaymentService", "run reconciliationID" );

	cybs_add( m_pMap, "payPalPreapprovedUpdateService", "run reconciliationID" );

	cybs_add( m_pMap, "riskUpdateService", "run actionCode recordID negativeAddress_city negativeAddress_state negativeAddress_postalCode negativeAddress_country" );

	cybs_add( m_pMap, "invoiceHeader", "merchantDescriptor merchantDescriptorContact isGift returnsAccepted tenderType merchantVATRegistrationNumber purchaserOrderDate purchaserVATRegistrationNumber vatInvoiceReferenceNumber summaryCommodityCode supplierOrderReference userPO costCenter purchaserCode taxable amexDataTAA1 amexDataTAA2 amexDataTAA3 amexDataTAA4 invoiceDate" );

	cybs_add( m_pMap, "businessRules", "ignoreAVSResult ignoreCVResult ignoreDAVResult ignoreExportResult ignoreValidateResult declineAVSFlags scoreThreshold" );

	cybs_add( m_pMap, "billTo", "title suffix city county state postalCode country company ipAddress ipNetworkAddress hostname domainName driversLicenseState customerID httpBrowserType httpBrowserCookiesAccepted" );

	cybs_add( m_pMap, "shipTo", "title suffix city county state postalCode country company shippingMethod" );

	cybs_add( m_pMap, "shipFrom", "title suffix city county state postalCode country company" );

	cybs_add( m_pMap, "card", "bin" );	

	cybs_add( m_pMap, "check", "" );
	
	cybs_add( m_pMap, "bml", "customerBillingAddressChange customerEmailChange customerHasCheckingAccount CustomerHasSavingsAccount customerPasswordChange customerPhoneChange customerRegistrationDate customerTypeFlag grossHouseholdIncome householdIncomeCurrency itemCategory merchantPromotionCode preapprovalNumber productDeliveryTypeIndicator residenceStatus tcVersion yearsAtCurrentResidence yearsWithCurrentEmployer employerCity employerCompanyName employerCountry employerPhoneType employerState employerPostalCode shipToPhoneType billToPhoneType" ); 

	cybs_add( m_pMap, "otherTax", "vatTaxAmount vatTaxRate alternateTaxAmount alternateTaxIndicator alternateTaxID localTaxAmount localTaxIndicator nationalTaxAmount nationalTaxIndicator" );
	
	cybs_add( m_pMap, "purchaseTotals", "currency discountAmount taxAmount dutyAmount grandTotalAmount freightAmount" );

	cybs_add( m_pMap, "fundingTotals", "currency grandTotalAmount" );

	cybs_add( m_pMap, "gecc", "saleType planNumber sequenceNumber promotionEndDate promotionPlan line" );

	cybs_add( m_pMap, "ucaf", "authenticationData collectionIndicator" );

	cybs_add( m_pMap, "fundTransfer", "" );

	cybs_add( m_pMap, "bankInfo", "bankCode name address city country branchCode swiftCode sortCode" );

	cybs_add( m_pMap, "recurringSubscriptionInfo", "status amount numberOfPayments numberOfPaymentsToAdd automaticRenew frequency startDate endDate approvalRequired event_amount event_approvedBy event_number billPayment" );

	cybs_add( m_pMap, "subscription", "title paymentMethod" );

	cybs_add( m_pMap, "decisionManager", "enabled profile" );

	cybs_add( m_pMap, "batch", "batchID recordID" );

	cybs_add( m_pMap, "payPal", "" );

	cybs_add( m_pMap, "jpo", "paymentMethod bonusAmount bonuses installments" );

	cybs_add( m_pMap, REQUEST_MESSAGE, "merchantID merchantReferenceCode clientLibrary clientLibraryVersion clientEnvironment clientSecurityLibraryVersion clientApplication clientApplicationVersion clientApplicationUser comments" );

	cybs_add( m_pMap, "ccAuthReply", "reasonCode amount avsCode avsCodeRaw cvCode cvCodeRaw authorizedDateTime processorResponse authFactorCode reconciliationID transactionToken fundingTotals_currency fundingTotals_grandTotalAmount fxQuoteID fxQuoteRate fxQuoteType fxQuoteExpirationDateTime" );

	cybs_add( m_pMap, "ccCaptureReply", "reasonCode requestDateTime amount reconciliationID transactionToken fundingTotals_currency fundingTotals_grandTotalAmount fxQuoteID fxQuoteRate fxQuoteType fxQuoteExpirationDateTime purchasingLevel3Enabled enhancedDataEnabled" );

	cybs_add( m_pMap, "ccCreditReply", "reasonCode requestDateTime amount reconciliationID transactionToken purchasingLevel3Enabled enhancedDataEnabled" );

	cybs_add( m_pMap, "ccAuthReversalReply", "reasonCode amount processorResponse requestDateTime transactionToken" );

	cybs_add( m_pMap, "ecDebitReply", "reasonCode settlementMethod requestDateTime amount verificationLevel reconciliationID processorResponse transactionToken avsCode avsCodeRaw" );

	cybs_add( m_pMap, "ecCreditReply", "reasonCode settlementMethod requestDateTime amount reconciliationID processorResponse transactionToken" );

	cybs_add( m_pMap, "payerAuthEnrollReply", "reasonCode acsURL commerceIndicator paReq proxyPAN xid proofXML ucafCollectionIndicator" );

	cybs_add( m_pMap, "payerAuthValidateReply", "reasonCode authenticationResult authenticationStatusMessage cavv commerceIndicator eci eciRaw xid ucafAuthenticationData ucafCollectionIndicator" );

	cybs_add( m_pMap, "taxReply", "reasonCode currency grandTotalAmount totalCityTaxAmount city totalCountyTaxAmount county totalDistrictTaxAmount totalStateTaxAmount state totalTaxAmount postalCode geocode item_cityTaxAmount item_countyTaxAmount item_districtTaxAmount item_stateTaxAmount item_totalTaxAmount" );

	cybs_add( m_pMap, "afsReply", "reasonCode afsResult hostSeverity consumerLocalTime afsFactorCode addressInfoCode hotlistInfoCode internetInfoCode phoneInfoCode suspiciousInfoCode velocityInfoCode" );

	cybs_add( m_pMap, "davReply", "reasonCode addressType apartmentInfo barCode barCodeCheckDigit cityInfo countryInfo directionalInfo lvrInfo matchScore standardizedCity standardizedCounty standardizedCSP standardizedState standardizedPostalCode standardizedCountry standardizedISOCountry stateInfo streetInfo suffixInfo postalCodeInfo overallInfo usInfo caInfo intlInfo usErrorInfo caErrorInfo intlErrorInfo" );

	cybs_add( m_pMap, "deniedPartiesMatch", "list" );
	
	cybs_add( m_pMap, "exportReply", "reasonCode ipCountryConfidence" );

	cybs_add( m_pMap, "fxRatesReply", "reasonCode quote_id quote_rate quote_type quote_expirationDateTime quote_currency quote_fundingCurrency quote_receivedDateTime" );

	cybs_add( m_pMap, "bankTransferReply", "reasonCode amount bankName bankCity bankCountry paymentReference processorResponse bankSwiftCode bankSpecialID requestDateTime reconciliationID" );

	cybs_add( m_pMap, "bankTransferRefundReply", "reasonCode amount requestDateTime reconciliationID processorResponse" );

	cybs_add( m_pMap, "directDebitReply", "reasonCode amount requestDateTime reconciliationID processorResponse" );

	cybs_add( m_pMap, "directDebitValidateReply", "reasonCode amount requestDateTime reconciliationID processorResponse" );

	cybs_add( m_pMap, "directDebitRefundReply", "reasonCode amount requestDateTime reconciliationID processorResponse" );

	cybs_add( m_pMap, "paySubscriptionCreateReply", "reasonCode" );

	cybs_add( m_pMap, "paySubscriptionUpdateReply", "reasonCode" );

	cybs_add( m_pMap, "paySubscriptionEventUpdateReply", "reasonCode" );

	cybs_add( m_pMap, "paySubscriptionRetrieveReply", "reasonCode approvalRequired automaticRenew cardType checkAccountType city comments companyName country currency customerAccountID endDate frequency merchantReferenceCode paymentMethod paymentsRemaining postalCode recurringAmount setupAmount startDate state status title totalPayments shipToCity shipToState shipToCompany shipToCountry billPayment merchantDefinedDataField1 merchantDefinedDataField2 merchantDefinedDateField3 merchantDefinedDataField4" );

	cybs_add( m_pMap, "payPalPaymentReply", "reasonCode amount requestDateTime reconciliationID" );

	cybs_add( m_pMap, "payPalCreditReply", "reasonCode amount requestDateTime reconciliationID processorResponse" );

	cybs_add( m_pMap, "voidReply", "reasonCode requestDateTime amount currency" );

	cybs_add( m_pMap, "pinlessDebitReply", "reasonCode amount requestDateTime processorResponse receiptNumber reconciliationID" );

	cybs_add( m_pMap, "pinlessDebitValidateReply", "reasonCode status requestDateTime" );

	cybs_add( m_pMap, "payPalButtonCreateReply", "reasonCode encryptedFormData requestDateTime reconciliationID buttonType" );

	cybs_add( m_pMap, "payPalPreapprovedPaymentReply", "reasonCode requestDateTime reconciliationID payerStatus transactionType feeAmount payerCountry pendingReason paymentStatus mpStatus payerBusiness desc mpMax paymentType paymentDate paymentGrossAmount settleAmount taxAmount exchangeRate paymentSourceID" );

	cybs_add( m_pMap, "payPalPreapprovedUpdateReply", "reasonCode requestDateTime reconciliationID payerStatus payerCountry mpStatus payerBusiness desc mpMax paymentSourceID" );

	cybs_add( m_pMap, "riskUpdateReply", "reasonCode" );

	cybs_add( m_pMap, "decisionReply", "activeProfileReply_selectedBy activeProfileReply_name activeProfileReply_destinationQueue activeProfileReply_rulesTriggered_ruleResultItem_name activeProfileReply_rulesTriggered_ruleResultItem_decision activeProfileReply_rulesTriggered_ruleResultItem_evaluation activeProfileReply_rulesTriggered_ruleResultItem_ruleID" );

	cybs_add( m_pMap, REPLY_MESSAGE, "merchantReferenceCode requestID decision reasonCode missingField invalidField" );

	cybs_add( m_pMap, "airlineData", "agentCode agentName ticketIssuerCity ticketIssuerState ticketIssuerPostalCode ticketIssuerCountry ticketIssuerCode ticketIssuerName ticketNumber checkDigit restrictedTicketIndicator transactionType extendedPaymentCode carrierName customerCode documentType documentNumber documentNumberOfParts chargeDetails bookingReference leg_carrierCode leg_flightNumber leg_originatingAirportCode leg_class leg_stopoverCode leg_departureDate leg_destination leg_fareBasis leg_departTax" );

	cybs_add( m_pMap, "pos", "entryMode cardPresent terminalCapability terminalID terminalType terminalLocation transactionSecurity catLevel conditionCode" );

	cybs_add( m_pMap, "merchantDefinedData", "field1 field2 field3 field4" );
}

SafeFields::~SafeFields() {
	if (m_pMap) { 
		cybs_destroy_map( m_pMap ); 
	}
}

// this is the maximum length that we can accomodate for the
// name of a field.  It should be long enough.  Of course, if
// you are adding a safe field that's longer than this, you
// would know to increase its value.  Otherwise, the only
// ill effect is that it would be automatically treated as
// unsafe and therefore would be masked in the logs.
static const unsigned int MAX_FIELD_LENGTH = 128;

bool SafeFields::IsSafe( SafeFields::MessageType eType, char *szField ) {
	if (!m_pMap) return( false );

	// if field is too long to accomodate in our buffer,
	// just treat it as unsafe.
	if (strlen( szField ) > MAX_FIELD_LENGTH) return( false );

	// strip-off any indices that are present in the field name
	// e.g. item_0_unitPrice will become item_unitPrice.
	char szStripped[MAX_FIELD_LENGTH+1];
	strcpy( szStripped, szField );
	RemoveIndices( szStripped );

	 char *szParent, *szChild;
     char *szUnderscore = strchr( szStripped, UNDERSCORE );

	 if (szUnderscore != NULL) {

		// split field into parent and child
		*szUnderscore = 0x00;
		szParent = szStripped;
		szChild = szUnderscore + 1;

        return( IsSafe( szParent, szChild ) );

      } else {
		// parent is either requestMessage or replyMessage
                return( IsSafe(
		           (char *) (eType == SafeFields::Request ? REQUEST_MESSAGE : REPLY_MESSAGE),
			   szStripped ) );
      }

}

bool SafeFields::IsSafe( char *szParent, char *szChild ) {
	
	if (!m_pMap) return( false );

	// get the list of safe fields for this parent
	const char *szList = (const char *)cybs_get( m_pMap, szParent );

	// if none, then this field is definitely not safe
	if (szList == NULL) return( false );

	// return whether or not this child is on the list
        return( strstr( szList, szChild ) != NULL );
}

// removes indices, e.g. item_0_unitPrice becomes item_unitPrice
void SafeFields::RemoveIndices( char *szField ) {

	char ch;
	int fDigit;
	int nIndexStart;
	FieldNameState state = SafeFields::Boundary;
	for (int nSrc = 0, nDest = 0; state != SafeFields::Done; ++nSrc) {
		ch = szField[nSrc];
		fDigit = isdigit( ch );
		switch( state ) {
			case SafeFields::Boundary:
				if (fDigit) {
					state = SafeFields::PossibleIndex;
					nIndexStart = nDest;
				} else {
					state = SafeFields::NonIndex;
				}
				break;
			case SafeFields::NonIndex:
				if (ch == UNDERSCORE) {
					state = SafeFields::Boundary;
				}
				break;
			case SafeFields::PossibleIndex:

				if (ch == UNDERSCORE || ch == 0x00) {
					// we found an index at the start of
					// the string; let's remove it and the
					// underscore after it by resetting
					// the dest position to 0 and ignoring
					// the underscore.
					if (nIndexStart == 0) {
						nDest = 0;
						if (ch == UNDERSCORE) continue;
					}
					// we found an index either in the middle
					// or at the end of the string; let's
					// remove it and the underscore before
					// it by resetting the dest position to
					// that of the underscore preceding the
					// index.
					else {
						nDest = nIndexStart - 1;
					}
					state = SafeFields::Boundary;

				} else if (!fDigit) {
					// it wasn't an index after all
					state = SafeFields::NonIndex;
				}
				// else if still a digit, then
				// it's still a possible index
				break;

			case Done:
				// do nothing;  in fact, this will
				// never happen since the loop would
				// exit right after state is set to Done.
				// This is just to suppress the gcc
				// warning about Done not being included
				// in the switch statement.  That was
				// actually a surprise.  Smart compiler. :)
				break;	
		} // switch

		szField[nDest++] = ch;
		if (ch == 0x00) {
			state = SafeFields::Done;
		}

	} // for

} // RemoveIndices
