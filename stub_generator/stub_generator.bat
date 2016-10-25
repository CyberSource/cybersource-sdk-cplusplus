@echo OFF
set /p targetAPIVersion= Enter name for targetAPIVersion (check https://ics2ws.ic3.com/commerce/1.x/transactionProcessor) ?

set /p wsdlType= Enter wsdl type (NVP or XML) ?

if /I "%wsdlType%"=="NVP" (
cd NVP
GOTO ISNVPWSDLEXIST

:CONTINUENVP
..\..\lib\gsoap-2.8\gsoap\bin\win32\wsdl2h.exe -I ..\..\lib\gsoap-2.8\gsoap\WS -f -k -o CyberSourceTransaction_nvp_%targetAPIVersion%.h CyberSourceTransaction_NVP_%targetAPIVersion%.wsdl

REM set /p DUMMY=Add #import "wsse.h" to CyberSourceTransaction_nvp_%targetAPIVersion%.h and hit ENTER to continue...

..\..\lib\gsoap-2.8\gsoap\bin\win32\soapcpp2 -C -L -j -I ..\..\lib\gsoap-2.8\gsoap\import -w -x CyberSourceTransaction_nvp_%targetAPIVersion%.h

set /p "copyDecision= Copy generated code to workspace (Y or N) ?"

if /I "%copyDecision%"=="Y" (

del ..\..\NVPClient\CyberSourceTransaction_nvp_*.h

echo copying CyberSourceTransaction_NVP_%targetAPIVersion%.h ...
copy /Y CyberSourceTransaction_NVP_%targetAPIVersion%.h ..\..\NVPClient

echo copying INVPTransactionProcessor.nsmap ...
copy /Y INVPTransactionProcessor.nsmap ..\..\NVPClient

echo copying soapC.cpp ...
copy /Y soapC.cpp ..\..\NVPClient

echo copying soapH.h ...
copy /Y soapH.h ..\..\NVPClient

echo copying soapINVPTransactionProcessorProxy.cpp ...
copy /Y soapINVPTransactionProcessorProxy.cpp ..\..\NVPClient

echo copying soapINVPTransactionProcessorProxy.h ...
copy /Y soapINVPTransactionProcessorProxy.h ..\..\NVPClient

echo copying soapStub ...
copy /Y soapStub.h ..\..\NVPClient

)
cd ..
EXIT /b
)

if /I "%wsdlType%"=="XML" (
cd XML
GOTO ISXMLWSDLEXIST

:CONTINUEXML
..\..\lib\gsoap-2.8\gsoap\bin\win32\wsdl2h.exe -I ..\..\lib\gsoap-2.8\gsoap\WS -f -k -o CyberSourceTransaction_xml_%targetAPIVersion%.h CyberSourceTransaction_%targetAPIVersion%.wsdl

REM set /p DUMMY=Add #import "wsse.h" to CyberSourceTransaction_xml_%targetAPIVersion%.h and hit ENTER to continue...

..\..\lib\gsoap-2.8\gsoap\bin\win32\soapcpp2 -C -L -j -I ..\..\lib\gsoap-2.8\gsoap\import -w -x CyberSourceTransaction_xml_%targetAPIVersion%.h

set /p "copyDecision= Copy generated code to workspace (Y or N) ?"

if /I "%copyDecision%"=="Y" (
del ..\..\XMLClient\CyberSourceTransaction_xml_*.h

echo copying CyberSourceTransaction_xml_%targetAPIVersion%.h ...
copy /Y CyberSourceTransaction_xml_%targetAPIVersion%.h ..\..\XMLClient

echo copying ITransactionProcessor.nsmap ...
copy /Y ITransactionProcessor.nsmap ..\..\XMLClient

echo copying soapC.cpp ...
copy /Y soapC.cpp ..\..\XMLClient

echo copying soapH.h ...
copy /Y soapH.h ..\..\XMLClient

echo copying soapITransactionProcessorProxy.cpp ...
copy /Y soapITransactionProcessorProxy.cpp ..\..\XMLClient

echo copying soapITransactionProcessorProxy.h ...
copy /Y soapITransactionProcessorProxy.h ..\..\XMLClient

echo copying soapStub ...
copy /Y soapStub.h ..\..\XMLClient
)
cd ..
EXIT /b
)

echo %wsdlType% is not a valid wsdl type.

:ISNVPWSDLEXIST
	if not exist CyberSourceTransaction_NVP_%targetAPIVersion%.wsdl (
	echo Error: wsdl file CyberSourceTransaction_NVP_%targetAPIVersion%.wsdl is not present.
	cd ..
	EXIT /b
	)
GOTO CONTINUENVP	

:ISXMLWSDLEXIST
	if not exist CyberSourceTransaction_%targetAPIVersion%.wsdl (
	echo Error: wsdl file CyberSourceTransaction_%targetAPIVersion%.wsdl is not present.
	cd ..
	EXIT /b
	)
GOTO CONTINUEXML