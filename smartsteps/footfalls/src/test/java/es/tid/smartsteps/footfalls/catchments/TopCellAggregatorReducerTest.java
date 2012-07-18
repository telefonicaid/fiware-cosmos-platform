package es.tid.smartsteps.footfalls.catchments;

import static java.util.Arrays.asList;

import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.mapreduce.BinaryKey;
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.Catchment;
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.Catchments;
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.TopCell;

/**
 *
 * @author sortega
 */
public class TopCellAggregatorReducerTest extends CatchmentsTestBase {

    private ReduceDriver<
        BinaryKey, TypedProtobufWritable<Catchments>,
        BinaryKey, TypedProtobufWritable<Catchments>> instance;

    @Before
    public void setUp() {
        this.instance = new ReduceDriver<
                BinaryKey, TypedProtobufWritable<Catchments>,
                BinaryKey, TypedProtobufWritable<Catchments>> (
                new TopCellAggregatorReducer());
    }

    @Test
    public void shouldAggregateTopCells() throws Exception {
        BinaryKey key = new BinaryKey("topLevelId", SAMPLE_DATE);
        TypedProtobufWritable<Catchments> expectedOutput =
                new TypedProtobufWritable<Catchments>(Catchments.newBuilder()
                    .setId("topLevelId")
                    .setDate(SAMPLE_DATE)
                    .setLatitude(0d)
                    .setLongitude(0d)
                    .addCatchments(Catchment.newBuilder()
                            .setHour(0)
                            .addAllTopCells(asList(this.topCell("0", 10d),
                                                   this.topCell("1", 10d))))
                    .addCatchments(Catchment.newBuilder()
                            .setHour(1)
                            .addAllTopCells(asList(this.topCell("2", 15d),
                                                   this.topCell("3", 15d))))
                    .build());
        this.instance
                .withInput(key, asList(
                    new TypedProtobufWritable<Catchments>(
                        this.singletonCatchment("topLevelId", 0, "0", 10d)),
                    new TypedProtobufWritable<Catchments>(
                        this.singletonCatchment("topLevelId", 0, "1", 10d)),
                    new TypedProtobufWritable<Catchments>(
                        this.singletonCatchment("topLevelId", 1, "2", 10d)),
                    new TypedProtobufWritable<Catchments>(
                        this.singletonCatchment("topLevelId", 1, "3",  5d)),                       new TypedProtobufWritable<Catchments>(
                        this.singletonCatchment("topLevelId", 1, "2",  5d)),
                    new TypedProtobufWritable<Catchments>(
                        this.singletonCatchment("topLevelId", 1, "3", 10d))
                ))
                .withOutput(key, expectedOutput)
                .runTest();
    }

    private TopCell topCell(String id, double count) {
        return TopCell.newBuilder()
                .setId(id)
                .setCount(count)
                .setLatitude(1d)
                .setLongitude(2d)
                .build();
    }
}
