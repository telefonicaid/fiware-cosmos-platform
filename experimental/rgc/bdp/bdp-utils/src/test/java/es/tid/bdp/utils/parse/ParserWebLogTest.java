package es.tid.bdp.utils.parse;

import static org.junit.Assert.assertEquals;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.BlockJUnit4ClassRunner;

import es.tid.ps.kpicalculation.data.WebLogPB.WebLog;

@RunWith(BlockJUnit4ClassRunner.class)
public class ParserWebLogTest {

    @Test
    public void parserMesageTest() {
        String pattern = "(^.+)\t(.*)\t(.*)\t(.*)\t(.*)\t(.*)\t(.*)";
        String attr = "visitorId|fullUrl|date|httpStatus|mimeType|userAgent|method";
        String line = "196971a1e4081456\thttp://api.facebook.com/restserver.php\t30Nov2010000005\t200\ttext/xml;charset=utf-8\t-FacebookConnect\tPOST";

        ParserWebLog parser = new ParserWebLog(pattern, attr);
        WebLog out = (WebLog) parser.parseLine(line);

        WebLog.Builder builder = WebLog.newBuilder();

        WebLog expected = builder.setVisitorId("196971a1e4081456")
                .setFullUrl("http://api.facebook.com/restserver.php")
                .setDate("30Nov2010000005").setHttpStatus("200")
                .setMimeType("text/xml;charset=utf-8")
                .setUserAgent("-FacebookConnect").setMethod("POST").build();
        
        assertEquals(expected, out);
    }

    @Test(expected = RuntimeException.class)
    public void noMatchTest() {
        String pattern = "(^.+)\t(.*)\t(.*)\t(.*)\t(.*)\t(.*)\t(.*)";
        String attr = "visitorId|fullUrl|date|httpStatus|mimeType|userAgent|method";
        String line = "***";

        ParserWebLog parser = new ParserWebLog(pattern, attr);
        parser.parseLine(line);
    }
}
