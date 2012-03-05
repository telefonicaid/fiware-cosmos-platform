package es.tid.bdp.utils.io.output;

import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.times;

import java.io.IOException;

import org.junit.Before;
import org.junit.Test;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufBlockWriter;

import es.tid.bdp.utils.parse.ParserAbstract;
import es.tid.ps.kpicalculation.data.WebLogPB.WebLog;

public class ProtoBufOutStreamTest {

    private ParserAbstract parser;

    @Before
    public void setup() {
        parser = (ParserAbstract) mock(ParserAbstract.class);

    }

    @Test
    public void parserMessageTest() throws IOException {
        ProtoBufOutStream out = new ProtoBufOutStream(null, parser);
        ProtobufBlockWriter<Message> writer = (ProtobufBlockWriter<Message>) mock(ProtobufBlockWriter.class);

        out.setWriter(writer);

        
        WebLog returned = WebLog.newBuilder().setVisitorId("196971a1e4081456")
                .setFullUrl("http://api.facebook.com/restserver.php")
                .setDate("30Nov2010000005").setHttpStatus("200")
                .setMimeType("text/xml;charset=utf-8")
                .setUserAgent("-FacebookConnect").setMethod("POST").build();
        when(parser.parseLine("test")).thenReturn(returned);

        String str = "test\n";
        byte[] buffer = str.getBytes();
        out.write(buffer, 0, buffer.length);
        
        
        verify(writer, times(1)).write(returned);

    }
    
    @Test
    public void parserMessage2partsLineTest() throws IOException {
        ProtoBufOutStream out = new ProtoBufOutStream(null, parser);
        ProtobufBlockWriter<Message> writer = (ProtobufBlockWriter<Message>) mock(ProtobufBlockWriter.class);

        out.setWriter(writer);

        
        WebLog returned = WebLog.newBuilder().setVisitorId("196971a1e4081456")
                .setFullUrl("http://api.facebook.com/restserver.php")
                .setDate("30Nov2010000005").setHttpStatus("200")
                .setMimeType("text/xml;charset=utf-8")
                .setUserAgent("-FacebookConnect").setMethod("POST").build();
        when(parser.parseLine("testaa")).thenReturn(returned);

        String str = "test";
        byte[] buffer = str.getBytes();
        out.write(buffer, 0, buffer.length);
        
        
        str = "aa\n";
        byte[] buffer2 = str.getBytes();
        out.write(buffer2, 0, buffer2.length);
        
        verify(writer, times(1)).write(returned);

    }

    
    @Test
    public void parserMessage2LinesTest() throws IOException {
        ProtoBufOutStream out = new ProtoBufOutStream(null, parser);
        ProtobufBlockWriter<Message> writer = (ProtobufBlockWriter<Message>) mock(ProtobufBlockWriter.class);

        out.setWriter(writer);

        
        WebLog returned1 = WebLog.newBuilder().setVisitorId("testaa")
                .setFullUrl("http://api.facebook.com/restserver.php")
                .setDate("30Nov2010000005").setHttpStatus("200")
                .setMimeType("text/xml;charset=utf-8")
                .setUserAgent("-FacebookConnect").setMethod("POST").build();
        when(parser.parseLine("testaa")).thenReturn(returned1);
        WebLog returned2 = WebLog.newBuilder().setVisitorId("testbb")
                .setFullUrl("http://api.facebook.com/restserver.php")
                .setDate("30Nov2010000005").setHttpStatus("200")
                .setMimeType("text/xml;charset=utf-8")
                .setUserAgent("-FacebookConnect").setMethod("POST").build();
        when(parser.parseLine("testbb")).thenReturn(returned2);

        String str = "test";
        byte[] buffer = str.getBytes();
        out.write(buffer, 0, buffer.length);
        
        
        str = "aa\ntestbb\n";
        byte[] buffer2 = str.getBytes();
        out.write(buffer2, 0, buffer2.length);
        
        verify(writer, times(1)).write(returned1);
        verify(writer, times(1)).write(returned2);

    }
    
    @Test
    public void closeBufferFullTest() throws IOException {
        ProtoBufOutStream out = new ProtoBufOutStream(null, parser);
        ProtobufBlockWriter<Message> writer = (ProtobufBlockWriter<Message>) mock(ProtobufBlockWriter.class);

        out.setWriter(writer);

        
        WebLog returned1 = WebLog.newBuilder().setVisitorId("testaa")
                .setFullUrl("http://api.facebook.com/restserver.php")
                .setDate("30Nov2010000005").setHttpStatus("200")
                .setMimeType("text/xml;charset=utf-8")
                .setUserAgent("-FacebookConnect").setMethod("POST").build();
        when(parser.parseLine("test")).thenReturn(returned1);

        String str = "test";
        byte[] buffer = str.getBytes();
        out.write(buffer, 0, buffer.length);
        
        out.close();
        
        verify(writer, times(1)).write(returned1);
    }
}
