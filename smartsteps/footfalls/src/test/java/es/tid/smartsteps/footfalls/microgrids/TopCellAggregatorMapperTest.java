package es.tid.smartsteps.footfalls.microgrids;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.mapreduce.BinaryKey;
import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.Catchments;

/**
 *
 * @author sortega
 */
public class TopCellAggregatorMapperTest {

    private MapDriver<
        Text, TypedProtobufWritable<Catchments>,
        BinaryKey, TypedProtobufWritable<Catchments>> instance;

    @Before
    public void setUp() {
        this.instance = new MapDriver<Text, TypedProtobufWritable<Catchments>,
                BinaryKey, TypedProtobufWritable<Catchments>>(
                new TopCellAggregatorMapper());
    }

    @Test
    public void shouldChangeKeyToIdAndDate() throws Exception {
        TypedProtobufWritable<Catchments> value =
                new TypedProtobufWritable<Catchments>(Catchments.newBuilder()
                .setId("topLevelId")
                .setDate("20120101")
                .setLatitude(0d)
                .setLongitude(0d)
                .build());

        this.instance
                .withInput(new Text("ignored"), value)
                .withOutput(new BinaryKey("topLevelId", "20120101"), value)
                .runTest();
    }
}
