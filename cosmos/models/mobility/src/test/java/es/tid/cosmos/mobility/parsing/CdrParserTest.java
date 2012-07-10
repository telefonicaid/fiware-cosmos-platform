package es.tid.cosmos.mobility.parsing;

import static org.junit.Assert.assertEquals;
import org.junit.*;

import es.tid.cosmos.mobility.data.CdrUtil;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Date;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Time;

/**
 *
 * @author sortega
 */
public class CdrParserTest {

    private DateParser dateParser;

    @Before
    public void setUp() throws Exception {
        this.dateParser = new DateParser("dd-mm-yyyy");
    }

    @Test
    public void testParse() throws Exception {
        CdrParser parser = new CdrParser("33F430521676F4|2221436242|"
                + "33F430521676F4|0442224173253|2|01/01/2010|02:00:01|2891|"
                + "RMITERR", "|", this.dateParser);
        assertEquals(CdrUtil.create(2221436242L, 0x521676f4,
                Date.newBuilder()
                .setDay(1)
                .setMonth(1)
                .setYear(2010)
                .setWeekday(5)
                .build(),
                Time.newBuilder()
                .setHour(2)
                .setMinute(0)
                .setSeconds(1)
                .build()),
                parser.parse());
    }
    
    @Test
    public void testParseMissingFirstUserId() throws Exception {
        CdrParser parser = new CdrParser("|2221436242|"
                + "33F430521676F4|0442224173253|2|01/01/2010|02:00:01|2891|"
                + "RMITERR", "|", this.dateParser);
        assertEquals(CdrUtil.create(2221436242L, 0x521676f4,
                Date.newBuilder()
                .setDay(1)
                .setMonth(1)
                .setYear(2010)
                .setWeekday(5)
                .build(),
                Time.newBuilder()
                .setHour(2)
                .setMinute(0)
                .setSeconds(1)
                .build()),
                parser.parse());
    }

    @Test
    public void testParseMissingSecondUserId() throws Exception {
        CdrParser parser = new CdrParser("33F430521676F4|2221436242|"
                + "|0442224173253|2|01/01/2010|02:00:01|2891|"
                + "RMITERR", "|", this.dateParser);
        assertEquals(CdrUtil.create(2221436242L, 0x521676f4,
                Date.newBuilder()
                .setDay(1)
                .setMonth(1)
                .setYear(2010)
                .setWeekday(5)
                .build(),
                Time.newBuilder()
                .setHour(2)
                .setMinute(0)
                .setSeconds(1)
                .build()),
                parser.parse());
    }
}
