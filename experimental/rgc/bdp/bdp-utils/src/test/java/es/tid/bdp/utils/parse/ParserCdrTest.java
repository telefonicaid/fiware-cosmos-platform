package es.tid.bdp.utils.parse;

import static org.junit.Assert.assertEquals;

import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.BlockJUnit4ClassRunner;

import es.tid.bdp.profile.data.CdrP.Cdr;
import es.tid.bdp.profile.data.CdrP.Date;
import es.tid.bdp.profile.data.CdrP.Time;

@RunWith(BlockJUnit4ClassRunner.class)
public class ParserCdrTest {

    private String pattern;
    private String attr;
    private ParserCdr instance;

    @Before
    public void setUp() {
        this.pattern = "(^.+)\\|(.*)\\|\\d\\|(\\d{2})/(\\d{2})/(\\d{4})\\|(\\d{2}):(\\d{2}):(\\d{2})\\|.*";
        this.attr = "userId|cellId|day|month|year|hour|minute|second";
        this.instance = new ParserCdr(pattern, attr);
    }

    @Test
    public void parserMesageTest() {
        String line = "523481101066|3481137831|2|01/04/2010|16:16:48|118-TELEFONIA MOVIL||MMS128";
        Cdr out = (Cdr) instance.parseLine(line);
        Cdr expected = Cdr
                .newBuilder()
                .setUserId(Long.parseLong("523481101066", 16))
                .setCellId(Long.parseLong("3481137831", 16))
                .setDate(
                        Date.newBuilder().setDay(1).setMonth(4).setYear(2010)
                                .setWeekday(5).build())
                .setTime(
                        Time.newBuilder().setHour(16).setMinute(16)
                                .setSeconds(48).build()).build();

        assertEquals(expected, out);
    }

    @Test
    public void parserMesageEmptyCellTest() {
        String alteranteAttr = "cellId|userId|day|month|year|hour|minute|second";
        String line = "523481101066||2|01/04/2010|16:16:48|118-TELEFONIA MOVIL||MMS128";
        ParserCdr parser = new ParserCdr(pattern, alteranteAttr);
        Cdr out = (Cdr) parser.parseLine(line);

        Cdr expected = Cdr
                .newBuilder()
                .setUserId(Long.parseLong("0", 16))
                .setCellId(Long.parseLong("523481101066", 16))
                .setDate(
                        Date.newBuilder().setDay(1).setMonth(4).setYear(2010)
                                .setWeekday(5).build())
                .setTime(
                        Time.newBuilder().setHour(16).setMinute(16)
                                .setSeconds(48).build()).build();

        assertEquals(expected, out);
    }


    @Test(expected = RuntimeException.class)
    public void parserMesageEmptyUserTest() {
        String line = "|3481137831|2|01/04/2010|16:16:48|118-TELEFONIA MOVIL||MMS128";
        instance.parseLine(line);
    }

    @Test(expected = RuntimeException.class)
    public void noMatchTest() {
        String line = "tt";
        instance.parseLine(line);
    }

    @Test(timeout = 1000)
    public void performanceTest() throws Exception {
        String line = "523481101066|3481137831|2|01/04/2010|16:16:48|118-TELEFONIA MOVIL||MMS128";
        for (int i = 0; i < 50000; i++) {
            instance.parseLine(line);
        }
    }
}
