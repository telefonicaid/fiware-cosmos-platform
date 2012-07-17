package es.tid.smartsteps.footfalls.microgrids;

import java.util.List;

import com.google.protobuf.Message;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;
import static org.mockito.Mockito.mock;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.Catchment;
import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.Catchments;
import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.TopCell;
import es.tid.smartsteps.footfalls.microgrids.data.generated.LookupProtocol.Lookup;

/**
 *
 * @author sortega
 */
public class TopCellScalerMapperTest {

    private MapDriver<
            Text, TypedProtobufWritable<Message>,
            Text, TypedProtobufWritable<Message>> instance;

    @Before
    public void setUp() {
        this.instance = new MapDriver<
                Text, TypedProtobufWritable<Message>,
                Text, TypedProtobufWritable<Message>>(
                        new TopCellScalerMapper());
    }

    @Test
    public void shouldMapLookup() throws Exception {
        Lookup lookup = Lookup.newBuilder()
                .setKey("src")
                .setValue("dst")
                .setProportion(0.4d)
                .build();
        TypedProtobufWritable<Message> wrappedMessage =
                new TypedProtobufWritable<Message>(lookup);
        this.instance
                .withInput(new Text("1234"), wrappedMessage)
                .withOutput(new Text("src"), wrappedMessage)
                .runTest();
    }

    @Test
    public void shouldMapTopCell() throws Exception {
        Catchments catchments = Catchments.newBuilder()
                .setId("topLevelId")
                .setDate("20121010")
                .setLatitude(0d)
                .setLongitude(0d)
                .addCatchments(Catchment.newBuilder()
                        .setHour(8)
                        .addTopCells(topCell("cell1", 30))
                        .addTopCells(topCell("cell2", 20))
                        .addTopCells(topCell("cell3", 10))
                )
                .build();
        TypedProtobufWritable<Message> wrappedMessage =
                new TypedProtobufWritable<Message>(catchments);

        List<Pair<Text, TypedProtobufWritable<Message>>> results =
                this.instance.withInput(new Text("1234"), wrappedMessage)
                             .run();

        assertEquals(3, results.size());
        assertEquals("cell1", results.get(0).getFirst().toString());
        assertEquals("cell2", results.get(1).getFirst().toString());
        assertEquals("cell3", results.get(2).getFirst().toString());

        Catchments emitted = (Catchments) results.get(0).getSecond().get();
        assertEquals(1, emitted.getCatchments(0).getTopCellsCount());
        assertEquals(topCell("cell1", 30),
                     emitted.getCatchments(0).getTopCells(0));
    }

    @Test(expected = IllegalStateException.class)
    public void shouldThrowOnUnexpectedMessage() throws Exception {
        Message message = mock(Message.class);
        this.instance
                .withInput(new Text("1234"),
                           new TypedProtobufWritable<Message>(message))
                .run();
    }

    private static TopCell topCell(String id, int count) {
        return TopCell.newBuilder()
                .setId(id)
                .setCount(count)
                .setLatitude(0d)
                .setLongitude(0d)
                .build();
    }
}
