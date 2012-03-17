package avrosandbox;

import java.io.*;
import org.apache.avro.Schema;
import org.apache.avro.generic.GenericData;
import org.apache.avro.generic.GenericDatumWriter;
import org.apache.avro.generic.GenericRecord;
import org.apache.avro.io.*;
import org.apache.avro.reflect.ReflectDatumReader;

public class AvroTest {

    public static void main (String argv[]) throws IOException {
        Schema schema = new Schema.Parser().parse(
                AvroTest.class.getResourceAsStream("individual_profile.avsc"));

        final GenericRecord sportCategory =
                new GenericData.Record(schema.getField("categories").schema().getElementType());
        sportCategory.put("name", "SPORT");
        sportCategory.put("count", 10l);

        GenericRecord profile = new GenericData.Record(schema);
        profile.put("userId", "id0001");
        profile.put("categories",
                new GenericData.Array(1, schema.getField("categories").schema())
                {{ add(0, sportCategory); }});

        ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
        Encoder encoder = EncoderFactory.get().binaryEncoder(outputStream, null);
        GenericDatumWriter writer = new GenericDatumWriter(schema);

        writer.write(profile, encoder);
        encoder.flush();

        String encodedString = outputStream.toString();
        System.out.println("encodedString: " + encodedString);

        // Read it back
        ByteArrayInputStream inputStream =
                new ByteArrayInputStream(outputStream.toByteArray());
        Decoder decoder = DecoderFactory.get().binaryDecoder(inputStream, null);
        ReflectDatumReader<IndividualProfile> profileReader =
                new ReflectDatumReader<IndividualProfile>(schema);
        IndividualProfile unmarshalledProfile = profileReader.read(null, decoder);
        System.out.println(unmarshalledProfile);
    }
}
