package avrosandbox;

import org.apache.avro.Schema;
import org.apache.avro.reflect.ReflectData;

public class AvroReflectionTest {

    public static void main(String argz[]) throws Exception {
        Schema schema = ReflectData.get().getSchema(IndividualProfile.class);
        System.out.println(schema.toString(true));
    }
}
