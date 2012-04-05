package es.tid.cosmos.profile.export.mongodb;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;

import com.mongodb.BasicDBObject;
import com.mongodb.DBObject;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

/**
 * Test case for MongoProperty
 *
 * @author sortega
 */
public class MongoPropertyTest {
    private MongoProperty instance;

    @Before
    public void setUp() {
        instance = new MongoProperty("prop", "value");
    }

    @Test
    public void shouldMergeWithDocument() throws Exception {
        DBObject keyDocument = new BasicDBObject();
        instance.appendAsKey(keyDocument);
        assertEquals("value", keyDocument.get("prop"));

        DBObject valueDocument = new BasicDBObject();
        instance.appendAsValue(valueDocument);
        assertEquals("value", valueDocument.get("prop"));
    }

    @Test
    public void serializationRoundTripTest() throws Exception {
        ByteArrayOutputStream output = new ByteArrayOutputStream();
        instance.write(new DataOutputStream(output));
        final byte[] serializedBytes = output.toByteArray();

        MongoProperty copy = new MongoProperty();
        final ByteArrayInputStream input =
                new ByteArrayInputStream(serializedBytes);
        copy.readFields(new DataInputStream(input));

        assertEquals("prop", copy.getProperty());
        assertEquals("value", copy.getValue());
    }

    @Test
    public void sortByValue() throws Exception {
        MongoProperty lower = new MongoProperty("prop", "valua");
        assertEquals(-4, lower.compareTo(instance));
        assertEquals( 4, instance.compareTo(lower));
        assertEquals( 0, instance.compareTo(instance));
    }
}
