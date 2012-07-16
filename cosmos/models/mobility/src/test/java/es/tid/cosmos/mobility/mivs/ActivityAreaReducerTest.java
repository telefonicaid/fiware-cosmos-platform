package es.tid.cosmos.mobility.mivs;

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.*;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobVars;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TelMonth;

/**
 *
 * @author logc
 */
public class ActivityAreaReducerTest {
    private static final double TOLERANCE = 0.1D;

    private ReduceDriver<ProtobufWritable<TelMonth>, TypedProtobufWritable<Cell>,
            LongWritable, TypedProtobufWritable<MobVars>> reducer;
    private Cell firstCell;
    private Cell secondCell;

    @Before
    public void setUp() {
        this.reducer = new ReduceDriver<ProtobufWritable<TelMonth>,
                TypedProtobufWritable<Cell>, LongWritable, TypedProtobufWritable<MobVars>>(
                        new ActivityAreaReducer());
        this.firstCell = CellUtil.create(590379901L, 100L, 1, 2,
                                         5000000D, 2000000D);
        this.secondCell = CellUtil.create(591266215L, 101L, 3, 4,
                                          7000000D, 4000000D);
    }

    @Test
    public void testEmitsAllVariables() throws IOException {
        ProtobufWritable<TelMonth> userWithSingleEntry =
                TelMonthUtil.createAndWrap(5512683500L, 1, true);
        TypedProtobufWritable<MobVars> outputWithAllVariables =
                new TypedProtobufWritable<MobVars>(MobVarsUtil.create(1, true, 1, 1,
                        1, 1, 5000000D, 2000000D, 0.0D, 0.0D));
        this.reducer
                .withInput(userWithSingleEntry,
                           asList(new TypedProtobufWritable<Cell>(this.firstCell)))
                .withOutput(new LongWritable(5512683500L),
                            outputWithAllVariables)
                .runTest();
    }

    @Test
    public void testCountsMakeSense() throws IOException {
        ProtobufWritable<TelMonth> userWithTwoEntries =
                TelMonthUtil.createAndWrap(5512684400L, 1, true);
        ProtobufWritable<MobVars> outputWithCorrectCounts =
                MobVarsUtil.createAndWrap(1, true, 2, 2, 2, 2, 6000000D,
                                          3000000D, Math.sqrt(2) * 1000000D,
                                          Math.sqrt(2) * 2000000D);
        List<Pair<LongWritable, TypedProtobufWritable<MobVars>>> res = this.reducer
                .withInput(userWithTwoEntries,
                           asList(new TypedProtobufWritable<Cell>(this.firstCell),
                                  new TypedProtobufWritable<Cell>(this.secondCell)))
                .run();
        TypedProtobufWritable<MobVars> resultWrapper = res.get(0).getSecond();
        final MobVars result = resultWrapper.get();
        assertEquals(outputWithCorrectCounts.get().getNumPos(),
                     result.getNumPos());
        assertEquals(outputWithCorrectCounts.get().getDifBtss(),
                     result.getDifBtss());
        assertEquals(outputWithCorrectCounts.get().getDifMuns(),
                     result.getDifMuns());
        assertEquals(outputWithCorrectCounts.get().getDifStates(),
                     result.getDifStates());
        assertEquals(outputWithCorrectCounts.get().getMasscenterUtmx(),
                     result.getMasscenterUtmx(), TOLERANCE);
        assertEquals(outputWithCorrectCounts.get().getMasscenterUtmy(),
                     result.getMasscenterUtmy(), TOLERANCE);
        assertEquals(outputWithCorrectCounts.get().getRadius(),
                     result.getRadius(), TOLERANCE);
        assertEquals(outputWithCorrectCounts.get().getDiamAreainf(),
                     result.getDiamAreainf(), TOLERANCE);
    }
}
