package es.tid.cosmos.base.data;

import com.google.protobuf.Message;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import org.junit.Test;

import es.tid.cosmos.base.data.generated.WebLogProtocol.WebLog;

/**
 *
 * @author dmicol
 */
public class MessageUtilTest {
    @Test
    public void testToString() {
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
        final String output = MessageUtil.toString(messageDescriptor, message);
        assertEquals("13213AB\thttp://www.google.com\t27/05/2012", output);
    }
}
