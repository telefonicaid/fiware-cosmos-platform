package es.tid.cosmos.base.data;

import com.google.protobuf.Message;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import org.junit.Test;

import es.tid.cosmos.base.data.generated.CdrProtocol.Uler;
import es.tid.cosmos.base.data.generated.WebLogProtocol.WebLog;

/**
 *
 * @author dmicol
 */
public class MessageGeneratorTest {
    @Test
    public void shouldGenerateUler() {
        MessageDescriptor messageDescriptor = new MessageDescriptor();
        messageDescriptor.setMetaFieldValue(
                MessageDescriptor.MetaFields.TYPE, "uler");
        messageDescriptor.setMetaFieldValue(
                MessageDescriptor.MetaFields.DELIMITER, " ");
        messageDescriptor.setFieldColumnIndex("hashed_uid", 0);
        messageDescriptor.setFieldColumnIndex("cell_lat", 2);
        messageDescriptor.setFieldColumnIndex("cell_long", 1);
        messageDescriptor.setFieldColumnIndex("event_day", 9);
        messageDescriptor.setFieldColumnIndex("event_time", 8);
        messageDescriptor.setFieldColumnIndex("cell_id", 10);
        messageDescriptor.setFieldColumnIndex("event_type", 11);
        messageDescriptor.setFieldColumnIndex("event_duration", 12);
        messageDescriptor.setFieldColumnIndex("age", 6);
        messageDescriptor.setFieldColumnIndex("gender", 7);
        messageDescriptor.setFieldColumnIndex("socio_eco_state", 3);
        messageDescriptor.setFieldColumnIndex("home_city", 4);
        messageDescriptor.setFieldColumnIndex("home_zip_code", 5);
        final String line = "ABC 0.5 76.3 unknown mutxamel 03110 28 male 18:00 "
                + "27/05/2012 DEF call 2:04";
        Message message = MessageGenerator.generate(messageDescriptor, line);
        assertTrue(message instanceof Uler);
        final Uler uler = (Uler)message;
        assertEquals("ABC", uler.getHashedUid());
        assertEquals("76.3", uler.getCellLat());
        assertEquals("0.5", uler.getCellLong());
        assertEquals("27/05/2012", uler.getEventDay());
        assertEquals("18:00", uler.getEventTime());
        assertEquals("DEF", uler.getCellId());
        assertEquals("call", uler.getEventType());
        assertEquals("2:04", uler.getEventDuration());
        assertEquals("28", uler.getAge());
        assertEquals("male", uler.getGender());
        assertEquals("unknown", uler.getSocioEcoState());
        assertEquals("mutxamel", uler.getHomeCity());
        assertEquals("03110", uler.getHomeZipCode());
    }

    @Test
    public void shouldGenerateWebLog() {
        MessageDescriptor messageDescriptor = new MessageDescriptor();
        messageDescriptor.setMetaFieldValue(
                MessageDescriptor.MetaFields.TYPE, "weblog");
        messageDescriptor.setMetaFieldValue(
                MessageDescriptor.MetaFields.DELIMITER, "\t");
        messageDescriptor.setFieldColumnIndex("user_id", 0);
        messageDescriptor.setFieldColumnIndex("url", 2);
        messageDescriptor.setFieldColumnIndex("date", 1);
        final String line = "13213AB\t27/05/2012\thttp://www.google.com";
        Message message = MessageGenerator.generate(messageDescriptor, line);
        assertTrue(message instanceof WebLog);
        final WebLog webLog = (WebLog)message;
        assertEquals("13213AB", webLog.getUserId());
        assertEquals("http://www.google.com", webLog.getUrl());
        assertEquals("27/05/2012", webLog.getDate());
    }

    @Test
    public void shouldGenerateWebLogWithUnespacedDelimiter() {
        MessageDescriptor messageDescriptor = new MessageDescriptor();
        messageDescriptor.setMetaFieldValue(
                MessageDescriptor.MetaFields.TYPE, "weblog");
        messageDescriptor.setMetaFieldValue(
                MessageDescriptor.MetaFields.DELIMITER, "\\t");
        messageDescriptor.setFieldColumnIndex("user_id", 0);
        messageDescriptor.setFieldColumnIndex("url", 2);
        messageDescriptor.setFieldColumnIndex("date", 1);
        final String line = "13213AB\t27/05/2012\thttp://www.google.com";
        Message message = MessageGenerator.generate(messageDescriptor, line);
        assertTrue(message instanceof WebLog);
        final WebLog webLog = (WebLog)message;
        assertEquals("13213AB", webLog.getUserId());
        assertEquals("http://www.google.com", webLog.getUrl());
        assertEquals("27/05/2012", webLog.getDate());
    }

    @Test
    public void shouldGenerateAnonymisedWebLog() {
        MessageDescriptor messageDescriptor = new MessageDescriptor();
        messageDescriptor.setMetaFieldValue(
                MessageDescriptor.MetaFields.TYPE, "weblog");
        messageDescriptor.setMetaFieldValue(
                MessageDescriptor.MetaFields.DELIMITER, "\t");
        messageDescriptor.setMetaFieldValue(
                MessageDescriptor.MetaFields.ANONYMISE, "user_id,date");
        messageDescriptor.setFieldColumnIndex("user_id", 0);
        messageDescriptor.setFieldColumnIndex("url", 2);
        messageDescriptor.setFieldColumnIndex("date", 1);
        final String line = "13213AB\t27/05/2012\thttp://www.google.com";
        Message message = MessageGenerator.generate(messageDescriptor, line);
        assertTrue(message instanceof WebLog);
        final WebLog webLog = (WebLog)message;
        assertEquals("b8ae72c1563ddffbcf652224797ed7704bd2129eca1fc6e56900f7ddb"
                + "1cd91e170e335ba65212ad2ed6f15a1eb2494b024efc783671bde830b420"
                + "f05587ea100", webLog.getUserId());
        assertEquals("http://www.google.com", webLog.getUrl());
        assertEquals("e86668a751a0a75814cb774bdcdfb53858bfba1df43de0168c525164b"
                + "e812afcd2851c9fed142efa6ce1fbda7fa4cf2d10ea633c06d77bb4a017b"
                + "a65d2bd827d", webLog.getDate());
    }

    @Test(expected=IllegalArgumentException.class)
    public void shouldFailOnInvalidType() {
        MessageDescriptor messageDescriptor = new MessageDescriptor();
        messageDescriptor.setMetaFieldValue(
                MessageDescriptor.MetaFields.TYPE, "invalid");
        messageDescriptor.setMetaFieldValue(
                MessageDescriptor.MetaFields.DELIMITER, "\t");
        final String line = "13213AB\t27/05/2012\thttp://www.google.com";
        MessageGenerator.generate(messageDescriptor, line);
    }

    @Test(expected=IllegalArgumentException.class)
    public void shouldFailOnMissingType() {
        MessageDescriptor messageDescriptor = new MessageDescriptor();
        messageDescriptor.setMetaFieldValue(
                MessageDescriptor.MetaFields.DELIMITER, " ");
        final String line = "13213AB\t27/05/2012\thttp://www.google.com";
        MessageGenerator.generate(messageDescriptor, line);
    }

    @Test(expected=IllegalArgumentException.class)
    public void shouldFailOnMissingDelimiter() {
        MessageDescriptor messageDescriptor = new MessageDescriptor();
        messageDescriptor.setMetaFieldValue(
                MessageDescriptor.MetaFields.TYPE, "invalid");
        final String line = "13213AB\t27/05/2012\thttp://www.google.com";
        MessageGenerator.generate(messageDescriptor, line);
    }

    @Test(expected=IllegalArgumentException.class)
    public void shouldFailOnInvalidColumnIndex() {
        MessageDescriptor messageDescriptor = new MessageDescriptor();
        messageDescriptor.setMetaFieldValue(
                MessageDescriptor.MetaFields.TYPE, "weblog");
        messageDescriptor.setMetaFieldValue(
                MessageDescriptor.MetaFields.DELIMITER, "\t");
        messageDescriptor.setFieldColumnIndex("user_id", 0);
        messageDescriptor.setFieldColumnIndex("url", 3);
        messageDescriptor.setFieldColumnIndex("date", 1);
        final String line = "13213AB\t27/05/2012\thttp://www.google.com";
        MessageGenerator.generate(messageDescriptor, line);
    }
}
