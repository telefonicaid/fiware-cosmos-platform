package es.tid.ps.mobility.jobs;

import static org.junit.Assert.assertEquals;
import org.junit.Test;

import es.tid.ps.mobility.data.BaseProtocol.Date;
import es.tid.ps.mobility.data.BaseProtocol.Time;
import es.tid.ps.mobility.data.MxCdrUtil;
import es.tid.ps.mobility.parser.CdrParser;

/**
 *
 * @author sortega
 */
public class CdrParserTest {
    @Test
    public void testParse() throws Exception {
        CdrParser parser = new CdrParser("33F430521676F4|2221436242|"
                + "33F430521676F4|0442224173253|2|01/01/2010|02:00:01|2891|RMITERR");
        assertEquals(MxCdrUtil.create(2221436242L, 0x521676f4,
                Date.newBuilder()
                .setDay(1)
                .setMonth(1)
                .setYear(10)
                .setWeekDay(5)
                .build(),
                Time.newBuilder()
                .setHour(2)
                .setMinute(0)
                .setSeconds(1)
                .build()),
                parser.parse());
    }
}
