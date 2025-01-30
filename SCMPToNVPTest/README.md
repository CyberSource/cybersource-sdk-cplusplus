# SCMPToNVPTest

This project contains sample code for converting SCMP requests to NTA (Simple Order API / SOAP), and then converting NTA responses back to SCMP. It also connects to the Simple Order API endpoint using p12 authentication, instead of SCMP.

This README.md file is specific to SCMPToNVPTest. For further details on the C++ SDK, refer to the main cybersource-sdk-cplusplus readme here: https://github.com/CyberSource/cybersource-sdk-cplusplus/blob/master/README.md.


## C++ SCMP to Simple Order Sample Client
1. Add the SCMP sample files to your project. The files mentioned here can be found in GitHub: https://github.com/CyberSource/cybersource-sdk-cplusplus/tree/scmp-sample/SCMPToNVPTest
1. Add a cybs.ini file to your application.
1. Update the cybs.ini file to contain the following list of values:
```properties
merchantID=
keysDirectory=
keyFilename=
serverURL=
password=
sslCertFile=
```
1. Other values may be set at your discretion. Use the cybersource-sdk-cplusplus readme file to determine which values you wish to change.
1. After your existing SCMP payment software creates a request payload, run the processRequest() method found in the ics_util.cpp class against that payload.
This method converts the SCMP payload into an Simple Order payload and forwards the Simple Order payload to the correct Simple Order endpoint. When the Simple Order reply payload is received, the method will then convert the payload back into an SCMP payload for normal processing within the system.
