package es.tid.bdp.utils;

import static org.junit.Assert.assertEquals;

import java.io.IOException;
import java.util.Properties;

import org.junit.Test;

import es.tid.bdp.utils.data.BdpFileDescriptor;

public class FileSystemControllerXmlTest {

    
    @Test(expected = RuntimeException.class)
    public void parserError() throws IOException {
        Properties properties = new Properties();
        properties.setProperty(FileSystemControllerXml.DESCRIPTOR_XML_FILE, "src/test/resources/error.xml");
        new FileSystemControllerXml(properties);        
    }
    
    @Test
    public void parserNoSuchElement() throws IOException {
        Properties properties = new Properties();
        properties.setProperty(FileSystemControllerXml.DESCRIPTOR_XML_FILE, "src/test/resources/correct.xml");
        FileSystemControllerXml builder = new FileSystemControllerXml(properties);
                
        BdpFileDescriptor out = builder.build("ruben", "/prueba/path");
        BdpFileDescriptor expected = new BdpFileDescriptor();

        
        expected.setCompressible(false);
        expected.setReadable(false);
        expected.setWritable(false);
        assertEquals(expected, out);

    }
    
    @Test
    public void parserUser() throws IOException {
        Properties properties = new Properties();
        properties.setProperty(FileSystemControllerXml.DESCRIPTOR_XML_FILE, "src/test/resources/correct.xml");
        FileSystemControllerXml builder = new FileSystemControllerXml(properties);
                
        BdpFileDescriptor out = builder.build("cdr", "/prueba");
        
        BdpFileDescriptor expected = new BdpFileDescriptor();
        
        expected.setCompressible(true);
        expected.setReadable(true);
        expected.setWritable(true);
        
        assertEquals(expected, out);
    }
    
    
    @Test
    public void parserUserPath() throws IOException {
        Properties properties = new Properties();
        properties.setProperty(FileSystemControllerXml.DESCRIPTOR_XML_FILE, "src/test/resources/correct.xml");
        FileSystemControllerXml builder = new FileSystemControllerXml(properties);
                
        BdpFileDescriptor out = builder.build("cdr", "/path");
        
        BdpFileDescriptor expected = new BdpFileDescriptor();
        
        expected.setCompressible(true);
        expected.setReadable(true);
        expected.setWritable(false);
        
        assertEquals(expected, out);
    }
}
