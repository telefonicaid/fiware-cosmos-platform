package es.tid.bdp.utils;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.NoSuchElementException;
import java.util.Properties;

import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;
import javax.xml.transform.Source;
import javax.xml.transform.stream.StreamSource;
import javax.xml.validation.SchemaFactory;

import org.xml.sax.Attributes;
import org.xml.sax.ErrorHandler;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.DefaultHandler;

import es.tid.bdp.utils.data.BdpFileDescriptor;
import es.tid.bdp.utils.parse.ParserAbstract;

public class FileSystemControllerXml extends FileSystemControllerAbstract {

    static final String DESCRIPTOR_XML_FILE = "descriptor.xml.file";

    private Map<String, BdpFileDescriptor> map;

    public FileSystemControllerXml(Properties properties) {
        super(properties);
        try {
            map = new HashMap<String, BdpFileDescriptor>();
            String path = properties.getProperty(DESCRIPTOR_XML_FILE);

            parserXML(path);
        } catch (Exception e) {
            throw new RuntimeException(e);
        }

    }

    private void parserXML(String path) throws ParserConfigurationException,
            SAXException, IOException {

        SAXParserFactory factory = SAXParserFactory.newInstance();
        factory.setValidating(false);
        factory.setNamespaceAware(true);

        SchemaFactory schemaFactory = SchemaFactory
                .newInstance("http://www.w3.org/2001/XMLSchema");

        factory.setSchema(schemaFactory
                .newSchema(new Source[] { new StreamSource(Thread
                        .currentThread().getContextClassLoader()
                        .getResourceAsStream("filesystem.xsd")) }));

        SAXParser parser = factory.newSAXParser();

        XMLReader reader = parser.getXMLReader();
        reader.setErrorHandler(new SimpleErrorHandler());
        reader.parse(new InputSource(path));

        System.out.println(factory.isValidating());

        SAXParser saxParser = factory.newSAXParser();
        DefaultHandler handler = new FileDescriptorHandler();

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

    private class FileDescriptorHandler extends DefaultHandler {

        String path;
        String user;
        BdpFileDescriptor descriptor;

        public void startElement(String uri, String localName, String qName,
                Attributes attributes) throws SAXException {

            if (qName.equalsIgnoreCase("description")) {
                path = attributes.getValue("path");
            }

            if (qName.equalsIgnoreCase("user")) {
                user = attributes.getValue("user");
                descriptor = new BdpFileDescriptor();
                descriptor.setCompressible(Boolean.parseBoolean(attributes
                        .getValue("isCompressible")));
                descriptor.setReadable(Boolean.parseBoolean(attributes
                        .getValue("isReadable")));
                descriptor.setWritable(Boolean.parseBoolean(attributes
                        .getValue("isWritable")));
            }

            if (qName.equalsIgnoreCase("parser")) {
                ParserAbstract parser = FileSystemControllerAbstract
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

    }

    private class SimpleErrorHandler implements ErrorHandler {
        public void warning(SAXParseException e) throws SAXException {
        }

        public void error(SAXParseException e) throws SAXException {
            System.out.println(e.getMessage());
            throw e;

        }

        public void fatalError(SAXParseException e) throws SAXException {
            System.out.println(e.getMessage());
        }
    }

}
