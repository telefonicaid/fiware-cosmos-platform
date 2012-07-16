package es.tid.cosmos.base.data;

import java.util.Set;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author dmicol
 */
public class MessageDescriptorTest {
    private MessageDescriptor instance;

    @Before
    public void setUp() {
        this.instance = new MessageDescriptor();
        this.instance.setMetaFieldValue(MessageDescriptor.MetaFields.TYPE,
                                        "weblog");
        this.instance.setMetaFieldValue(MessageDescriptor.MetaFields.DELIMITER,
                                        "\t");
        this.instance.setMetaFieldValue(MessageDescriptor.MetaFields.ANONYMISE,
                                        "user_id,date");
        this.instance.setFieldColumnIndex("user_id", 0);
        this.instance.setFieldColumnIndex("url", 2);
        this.instance.setFieldColumnIndex("date", 1);
    }

    @Test
    public void testShouldAnonymiseField() {
        assertTrue(this.instance.shouldAnonymiseField("user_id"));
        assertFalse(this.instance.shouldAnonymiseField("url"));
        assertTrue(this.instance.shouldAnonymiseField("date"));
    }

    @Test
    public void testGetFieldColumnIndex() {
        assertEquals(0, this.instance.getFieldColumnIndex("user_id"));
        assertEquals(2, this.instance.getFieldColumnIndex("url"));
        assertEquals(1, this.instance.getFieldColumnIndex("date"));
    }

    @Test(expected=IllegalArgumentException.class)
    public void testGetFieldColumnIndexForMissingField() {
        this.instance.getFieldColumnIndex("fake_field");
    }

    @Test
    public void testGetFieldNames() {
        Set<String> fieldNames = this.instance.getFieldNames();
        assertEquals(3, fieldNames.size());
        assertTrue(fieldNames.contains("user_id"));
        assertTrue(fieldNames.contains("url"));
        assertTrue(fieldNames.contains("date"));
    }

    @Test
    public void testSetFieldColumnIndex() {
        this.instance.setFieldColumnIndex("fake_field", 3);
        assertEquals(3, this.instance.getFieldColumnIndex("fake_field"));
    }

    @Test(expected=IllegalArgumentException.class)
    public void testSetFieldColumnIndexForRepeatedField() {
        this.instance.setFieldColumnIndex("date", 2);
    }
}
