API_VERSION    = 6.0.4
PROD_NAME      = simapi-c
ZIP_NAME       = $(PROD_NAME)-linux
DIST_BASE      = builds
DIST           = $(DIST_BASE)/$(PROD_NAME)-$(API_VERSION)

build32:
	$(MAKE) -C BaseClient
	$(MAKE) -C BaseClient static
	$(MAKE) -C NVPClient 32
	$(MAKE) -C XMLClient 32

build64:
	$(MAKE) -C BaseClient
	$(MAKE) -C BaseClient static
	$(MAKE) -C NVPClient 64
	$(MAKE) -C XMLClient 64

clean:
	$(MAKE) -C BaseClient clean
	$(MAKE) -C NVPClient clean
	$(MAKE) -C XMLClient clean

dist64:
	$(MAKE) dist
	cp lib/libxml2/linux64/lib/libxml2.a $(DIST)/lib
	cp lib/libxml2/linux64/lib/libxml2.so.2.9.11  $(DIST)/lib
	cp lib/openssl-1.1.1l/linux64/lib/libssl.so.1.1 $(DIST)/lib
	cp lib/openssl-1.1.1l/linux64/lib/libcrypto.so.1.1 $(DIST)/lib
	@echo Done building package: $(DIST_BASE)/$(ZIP_NAME)-$(API_VERSION).tar.gz
        
dist32: 
	$(MAKE) dist
	cp lib/libxml2/linux32/lib/libxml2.a $(DIST)/lib
	cp lib/libxml2/linux32/lib/libxml2.so.2.9.11  $(DIST)/lib
	cp lib/openssl-1.1.1l/linux32/lib/libssl.so.1.1 $(DIST)/lib
	cp lib/openssl-1.1.1l/linux32/lib/libcrypto.so.1.1 $(DIST)/lib
	@echo Done building package: $(DIST_BASE)/$(ZIP_NAME)-$(API_VERSION).tar.gz
		
dist:
	mkdir -p $(DIST_BASE)
	mkdir -p $(DIST)
	rm -f -r $(DIST)/*
	mkdir -p $(DIST)/samples
	cp resources/cybs.ini $(DIST)/samples/cybs.ini
	cp resources/auth.xml $(DIST)/samples/auth.xml
	
	mkdir -p $(DIST)/samples/nvp
	cp NVPTest/compile.sh $(DIST)/samples/nvp
	cp NVPTest/NVPTest.cpp $(DIST)/samples/nvp
	cp NVPTest/NVPTest.o $(DIST)/samples/nvp
	cp NVPTest/NVPTest $(DIST)/samples/nvp

	mkdir -p $(DIST)/samples/xml
	cp xml/compile.sh $(DIST)/samples/xml
	cp xml/xml.cpp $(DIST)/samples/xml
	cp xml/xml $(DIST)/samples/xml

	mkdir -p $(DIST)/lib
        
	cp NVPClient/libNVPClient.so $(DIST)/lib
	cp XMLClient/libXMLClient.so $(DIST)/lib
	cp BaseClient/libbaseclient.so $(DIST)/lib
	cp NVPClient/libNVPClient.a $(DITS)/lib
	cp XMLClient/libXMLClient.a $(DIST)/lib
	cp BaseClient/libbaseclient.a $(DIST)/lib

	mkdir -p $(DIST)/include/NVPClient
	cp NVPClient/NVPCybersource.h $(DIST)/include/NVPClient

	mkdir -p $(DIST)/include/XMLClient
	cp XMLClient/XMLCybersource.h $(DIST)/include/XMLClient
	cp XMLClient/soapStub.h $(DIST)/include/XMLClient
	cp XMLClient/soapITransactionProcessorProxy.h $(DIST)/include/XMLClient
	cp XMLClient/stdsoap2.h $(DIST)/include/XMLClient
	cp XMLClient/soapH.h $(DIST)/include/XMLClient
	
	mkdir -p $(DIST)/include/XMLClient
	
	mkdir -p $(DIST)/include/Common
	cp BaseClient/util.h $(DIST)/include/Common
	mkdir -p $(DIST)/keys
	cp resources/ca-bundle.crt $(DIST)/keys

	rm -f $(ZIP_NAME)-$(API_VERSION).tar.gz
	cd $(DIST_BASE); tar -zcvf $(ZIP_NAME)-$(API_VERSION).tar.gz $(PROD_NAME)-$(API_VERSION)

