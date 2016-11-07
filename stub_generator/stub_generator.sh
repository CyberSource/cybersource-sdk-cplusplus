echo -n "Enter name for targetAPIVersion (check https://ics2ws.ic3.com/commerce/1.x/transactionProcessor) ? > "
read targetAPIVersion

echo -n "Enter wsdl type (NVP or XML) ? "
read wsdlType

if [ "$wsdlType" == "NVP" ]
then
	cd NVP

	if [ -f "CyberSourceTransaction_NVP_$targetAPIVersion.wsdl" ]
        then
     		echo "Ready to generate stub"           
        else
                echo "Error: wsdl file CyberSourceTransaction_NVP_$targetAPIVersion.wsdl is not present."
		exit
	fi


	../../lib/gsoap-2.8/gsoap/wsdl/wsdl2h -I ../../lib/gsoap-2.8/gsoap/WS -f -k -o CyberSourceTransaction_nvp_$targetAPIVersion.h CyberSourceTransaction_NVP_$targetAPIVersion.wsdl

	../../lib/gsoap-2.8/gsoap/src/soapcpp2 -C -L -j -I ../../lib/gsoap-2.8/gsoap/import -w -x CyberSourceTransaction_nvp_$targetAPIVersion.h

	echo -n "Copy generated code to workspace (Y or N) ?"
	read copyDecision

	if [ "$copyDecision" == "Y" ]
	then

		rm -rf ../../NVPClient/CyberSourceTransaction_nvp_*.h

		echo copying CyberSourceTransaction_NVP_$targetAPIVersion.h ...
		cp -r CyberSourceTransaction_nvp_$targetAPIVersion.h ../../NVPClient

		echo copying INVPTransactionProcessor.nsmap ...
		cp -r INVPTransactionProcessor.nsmap ../../NVPClient

		echo copying soapC.cpp ...
		cp -r soapC.cpp ../../NVPClient

		echo copying soapH.h ...
		cp -r soapH.h ../../NVPClient

		echo copying soapINVPTransactionProcessorProxy.cpp ...
		cp -r soapINVPTransactionProcessorProxy.cpp ../../NVPClient

		echo copying soapINVPTransactionProcessorProxy.h ...
		cp -r soapINVPTransactionProcessorProxy.h ../../NVPClient

		echo copying soapStub ...
		cp -r soapStub.h ../../NVPClient


	fi
	exit
fi

if [ "$wsdlType" == "XML" ]
then
	cd XML
	
	if [ -f "CyberSourceTransaction_$targetAPIVersion.wsdl" ]
        then
                echo "Ready to generate stub"
        else
                echo "Error: wsdl file CyberSourceTransaction_$targetAPIVersion.wsdl is not present."
                exit
        fi

	../../lib/gsoap-2.8/gsoap/wsdl/wsdl2h -I ../../lib/gsoap-2.8/gsoap/WS -f -k -o CyberSourceTransaction_xml_$targetAPIVersion.h CyberSourceTransaction_$targetAPIVersion.wsdl
	
	../../lib/gsoap-2.8/gsoap/src/soapcpp2 -C -L -j -I ../../lib/gsoap-2.8/gsoap/import -w -x CyberSourceTransaction_xml_$targetAPIVersion.h
	
	echo -n "Copy generated code to workspace (Y or N) ?"
	read copyDecision
	
	if [ "$copyDecision" == "Y" ]
	then
	
		rm -rf ../../XMLClient/CyberSourceTransaction_xml_*.h
	
		echo copying CyberSourceTransaction_xml_$targetAPIVersion.h ...
		cp -r CyberSourceTransaction_xml_$targetAPIVersion.h ../../XMLClient
	
		echo copying ITransactionProcessor.nsmap ...
		cp -r ITransactionProcessor.nsmap ../../XMLClient
	
		echo copying soapC.cpp ...
		cp -r soapC.cpp ../../XMLClient
	
		echo copying soapH.h ...
		cp -r soapH.h ../../XMLClient
	
		echo copying soapITransactionProcessorProxy.cpp ...
		cp -r soapITransactionProcessorProxy.cpp ../../XMLClient
	
		echo copying soapITransactionProcessorProxy.h ...
		cp -r soapITransactionProcessorProxy.h ../../XMLClient
	
		echo copying soapStub ...
		cp -r soapStub.h ../../XMLClient
	
	fi
	exit
fi

echo $wsdlType is not a valid wsdl type.


