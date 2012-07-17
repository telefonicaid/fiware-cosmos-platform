package es.tid.smartsteps.footfalls.microgrids;

import java.io.IOException;
import static java.util.Arrays.asList;

import com.google.protobuf.Message;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.Catchments;
import es.tid.smartsteps.footfalls.microgrids.data.generated.LookupProtocol.Lookup;

/**
 *
 * @author sortega
 */
public class TopCellScalerReducerTest extends CatchmentsBasedTest {

    private ReduceDriver<
            Text, TypedProtobufWritable<Message>,
            Text, TypedProtobufWritable<Catchments>> instance;

    public TopCellScalerReducerTest() throws IOException {}

    @Before
    public void setUp() {
        this.instance = new ReduceDriver<
                Text, TypedProtobufWritable<Message>,
                Text, TypedProtobufWritable<Catchments>>(
                        new TopCellScalerReducer());
    }

    @Test
    public void shouldScaleCounts() throws Exception {
        Lookup lookup = Lookup.newBuilder()
                .setKey("src")
                .setValue("dst")
                .setProportion(0.4d)
                .build();
        Text key = new Text("src");
        this.instance.withInput(key,
                asList(new TypedProtobufWritable<Message>(lookup),
                       new TypedProtobufWritable<Message>(
                        this.singletonCatchment("topLevelId", 22, "src", 100d))))
                .withOutput(key, new TypedProtobufWritable<Catchments>(
                        this.singletonCatchment("topLevelId", 22, "dst", 40d)))
                .runTest();
    }

    @Test
    public void shouldHaveMultipleWrites() throws Exception {
        Lookup lookup1 = Lookup.newBuilder()
                .setKey("src")
                .setValue("dst1")
                .setProportion(0.4d)
                .build();
        Lookup lookup2 = Lookup.newBuilder()
                .setKey("src")
                .setValue("dst2")
                .setProportion(0.6d)
                .build();
        Text key = new Text("src");

        int results = this.instance.withInput(key,
                asList(new TypedProtobufWritable<Message>(lookup1),
                       new TypedProtobufWritable<Message>(lookup2),
                       new TypedProtobufWritable<Message>(
                        this.singletonCatchment("tld1", 22, "src", 100d)),
                       new TypedProtobufWritable<Message>(
                        this.singletonCatchment("tld2",  8, "src",  10d)),
                       new TypedProtobufWritable<Message>(
                        this.singletonCatchment("tld3",  8, "src", 200d))))
                .run().size();
        assertEquals(6, results);
    }

}
