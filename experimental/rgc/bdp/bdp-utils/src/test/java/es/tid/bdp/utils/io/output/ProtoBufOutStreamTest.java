package es.tid.bdp.utils.io.output;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;

import com.twitter.elephantbird.mapreduce.io.ProtobufBlockReader;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import com.twitter.elephantbird.util.TypeRef;
import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

import es.tid.bdp.utils.parse.ParserAbstract;
import es.tid.ps.kpicalculation.data.WebLogPB.WebLog;

public class ProtoBufOutStreamTest {

    private ParserAbstract parser;
    private ByteArrayOutputStream output;
    private ProtoBufOutStream instance;

    @Before
    public void setup() {
        this.parser = mock(ParserAbstract.class);
        this.output = new ByteArrayOutputStream();
        this.instance = new ProtoBufOutStream(output, parser);
    }

    @Test
    public void parserMessageTest() throws Exception {
        WebLog returned = WebLog.newBuilder().setVisitorId("196971a1e4081456")
                .setFullUrl("http://api.facebook.com/restserver.php")
                .setDate("30Nov2010000005").setHttpStatus("200")
                .setMimeType("text/xml;charset=utf-8")
                .setUserAgent("-FacebookConnect").setMethod("POST").build();
        when(parser.parseLine("test")).thenReturn(returned);

        String str = "test\n";
        byte[] buffer = str.getBytes();
        instance.write(buffer, 0, buffer.length);
        instance.close();

        assertOutput(returned);
    }

    @Test
    public void parserMessage2partsLineTest() throws Exception {
        WebLog returned = WebLog.newBuilder().setVisitorId("196971a1e4081456")
                .setFullUrl("http://api.facebook.com/restserver.php")
                .setDate("30Nov2010000005").setHttpStatus("200")
                .setMimeType("text/xml;charset=utf-8")
                .setUserAgent("-FacebookConnect").setMethod("POST").build();
        when(parser.parseLine("testaa")).thenReturn(returned);

        String str = "test";
        byte[] buffer = str.getBytes();
        instance.write(buffer, 0, buffer.length);


        str = "aa\n";
        byte[] buffer2 = str.getBytes();
        instance.write(buffer2, 0, buffer2.length);

        instance.close();

        assertOutput(returned);
    }


    @Test
    public void parserMessage2LinesTest() throws Exception {
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
        instance.write(buffer, 0, buffer.length);


        str = "aa\ntestbb\n";
        byte[] buffer2 = str.getBytes();
        instance.write(buffer2, 0, buffer2.length);

        instance.close();

        assertOutput(returned1, returned2);
    }

    @Test
    public void closeBufferFullTest() throws Exception {
        WebLog returned1 = WebLog.newBuilder().setVisitorId("testaa")
                .setFullUrl("http://api.facebook.com/restserver.php")
                .setDate("30Nov2010000005").setHttpStatus("200")
                .setMimeType("text/xml;charset=utf-8")
                .setUserAgent("-FacebookConnect").setMethod("POST").build();
        when(parser.parseLine("test")).thenReturn(returned1);

        String str = "test";
        byte[] buffer = str.getBytes();
        instance.write(buffer, 0, buffer.length);
        instance.close();

        assertOutput(returned1);
    }

    private void assertOutput(WebLog... expectedWebLogs) throws Exception {
        ByteArrayInputStream input = new ByteArrayInputStream(this.output.toByteArray());
        ProtobufBlockReader<WebLog> reader = new ProtobufBlockReader<WebLog>(input,
                new TypeRef(WebLog.class) {});
        ProtobufWritable<WebLog> wrapper = ProtobufWritable.newInstance(WebLog.class);
        for (WebLog expected : expectedWebLogs) {
            assertTrue("Expected message" + expected + "but none was found",
                    reader.readProtobuf(wrapper));
            assertEquals(expected, wrapper.get());
        }
        boolean hasMore = reader.readProtobuf(wrapper);
        assertFalse("No more messages expected but " + wrapper.get() +
                "was found", hasMore);
    }
}
