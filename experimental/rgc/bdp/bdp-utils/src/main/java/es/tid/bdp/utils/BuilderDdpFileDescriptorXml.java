package es.tid.bdp.utils;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.NoSuchElementException;

import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

import es.tid.bdp.utils.data.BdpFileDescriptor;
import es.tid.bdp.utils.parse.ParserAbstract;

public class BuilderDdpFileDescriptorXml extends
        BuilderDdpFileDescriptorAbstract {

    private static final String DESCRIPTOR_XML_FILE = "descriptor.xml.file";
    private Map<String, BdpFileDescriptor> map;

    public BuilderDdpFileDescriptorXml(PropertiesPlaceHolder proterties) {
        super(proterties);
        try {
            map = new HashMap<String, BdpFileDescriptor>();
            String path = proterties.getProperty(DESCRIPTOR_XML_FILE);
            parserXML(path);
        } catch (Exception e) {
            // TODO: handle exception
        }
    }

    private void parserXML(String path) throws ParserConfigurationException,
            SAXException, IOException {

        SAXParserFactory factory = SAXParserFactory.newInstance();
        SAXParser saxParser = factory.newSAXParser();
        DefaultHandler handler = new DefaultHandler() {

            String path;
            String user;
            BdpFileDescriptor descriptor;

            public void startElement(String uri, String localName,
                    String qName, Attributes attributes) throws SAXException {

                if (qName.equalsIgnoreCase("description")) {
                    path = attributes.getValue("path");
                }

                if (qName.equalsIgnoreCase("user")) {
                    user  = attributes.getValue("user");
                    descriptor = new BdpFileDescriptor();
                    descriptor.setCompressible(Boolean.parseBoolean(attributes
                            .getValue("isCompressible")));
                    descriptor.setReadable(Boolean.parseBoolean(attributes
                            .getValue("isReadable")));
                    descriptor.setWritable(Boolean.parseBoolean(attributes
                            .getValue("isWritable")));
                }

                if (qName.equalsIgnoreCase("parser")) {
                    ParserAbstract parser = BuilderDdpFileDescriptorAbstract
                            .createParser(attributes.getValue("className"),
                                    attributes.getValue("pattern"),
                                    attributes.getValue("attr"));
                    descriptor.setParser(parser);
                }
            }

            public void endElement(String uri, String localName, String qName)
                    throws SAXException {

                if (qName.equalsIgnoreCase("user")) {
                    map.put(path + "#" + user, descriptor);
                }
            }

            public void characters(char ch[], int start, int length)
                    throws SAXException {
            }

        };

        saxParser.parse(path, handler);

    }


    @Override
    protected BdpFileDescriptor searchFileDescriptor(String user, String path) {
        BdpFileDescriptor descriptor = map.get(path + "#" + user);
        if (descriptor != null) {
            return descriptor;
        } else {
            throw new NoSuchElementException();
        }
    }

}
