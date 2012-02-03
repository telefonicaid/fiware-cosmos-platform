all : CDRProtocol.java WebNavigationProtocol.java

CDRProtocol.java : CDRProtocol.avpr
	java -jar avro-tools-1.6.1.jar compile schema $< $@

CDRProtocol.avpr : CDRProtocol.avdl
	java -jar avro-tools-1.6.1.jar idl $< > $@

WebNavigationProtocol.java : WebNavigationProtocol.avpr
	java -jar avro-tools-1.6.1.jar compile schema $< $@

WebNavigationProtocol.avpr : WebNavigationProtocol.avdl
	java -jar avro-tools-1.6.1.jar idl $< > $@

clean :
	rm *.java *.avpr
