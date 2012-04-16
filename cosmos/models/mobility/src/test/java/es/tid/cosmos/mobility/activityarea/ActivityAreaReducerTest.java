package es.tid.cosmos.mobility.activityarea;

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Test;
import static org.junit.Assert.*;

import es.tid.cosmos.mobility.data.ActivityAreaUtil;
import es.tid.cosmos.mobility.data.ActivityAreaKeyUtil;
import es.tid.cosmos.mobility.data.CellUtil;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityArea;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityAreaKey;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;


/**
 *
 * @author losa
 */
public class ActivityAreaReducerTest {
    private ReduceDriver<
        ProtobufWritable<ActivityAreaKey>, ProtobufWritable<Cell>,
        ProtobufWritable<ActivityAreaKey>, ProtobufWritable<ActivityArea>>
        reducer;
    private ProtobufWritable<Cell> firstCell;
    private ProtobufWritable<Cell> secondCell;
    private double TOLERANCE;

    @Before
    public void setUp() {
        this.reducer = new ReduceDriver<
            ProtobufWritable<ActivityAreaKey>, ProtobufWritable<Cell>,
            ProtobufWritable<ActivityAreaKey>, ProtobufWritable<ActivityArea>>(
                    new ActivityAreaReducer());
        this.TOLERANCE = 0.1;

        double firstPosX = 5000000D;
        double secondPosX = 7000000D;
        double firstPosY = 2000000D;
        double secondPosY = 4000000D;

        this.firstCell = CellUtil.createAndWrap(590379901L, 100L,
                1, 2,
                firstPosX, firstPosY);
        this.secondCell = CellUtil.createAndWrap(591266215L, 101L,
                3, 4,
                secondPosX, secondPosY);
    }

    @Test
    public void testEmitsAllVariables() throws IOException {
        ProtobufWritable<ActivityAreaKey> userWithSingleEntry =
            ActivityAreaKeyUtil.createAndWrap(5512683500L, 1, true);

        int numPos = 1;
        int difBtss = 1;
        int difMuns = 1;
        int difStates = 1;
        double masscenterUtmX = 5000000D;
        double masscenterUtmY = 2000000D;
        double radius = 0.0;
        double diamAreaInf = 0.0;

        ProtobufWritable<ActivityArea> outputWithAllVariables = 
            ActivityAreaUtil.createAndWrap(numPos, difBtss, difMuns,
                    difStates, masscenterUtmX, masscenterUtmY, radius,
                    diamAreaInf);
        this.reducer
                .withInput(userWithSingleEntry, asList(this.firstCell))
                .withOutput(userWithSingleEntry, outputWithAllVariables)
                .runTest();
    }

    @Test
    public void testCountsMakeSense() throws IOException {
        ProtobufWritable<ActivityAreaKey> userWithTwoEntries =
            ActivityAreaKeyUtil.createAndWrap(5512684400L, 1, true);

        int numPos = 2;
        int difBtss = 2;
        int difMuns = 2;
        int difStates = 2;
        double masscenterUtmX = 6000000D;
        double masscenterUtmY = 3000000D;
        double radius = Math.sqrt(2)*1000000;
        double diamAreaInf = Math.sqrt(2)*2000000;

        ProtobufWritable<ActivityArea> outputWithCorrectCounts =
            ActivityAreaUtil.createAndWrap(numPos, difBtss, difMuns, difStates,
                    masscenterUtmX, masscenterUtmY, radius,
                    diamAreaInf);
        List<Pair<ProtobufWritable<ActivityAreaKey>,
            ProtobufWritable<ActivityArea>>> results =
                this.reducer
                        .withInput(userWithTwoEntries,
                                   asList(this.firstCell, this.secondCell))
                        .withOutput(userWithTwoEntries,
                                    outputWithCorrectCounts)
                        .run();

        ProtobufWritable<ActivityArea> resultValue =
                results.get(0).getSecond();
        resultValue.setConverter(ActivityArea.class);
        assertEquals(outputWithCorrectCounts.get().getNumPos(),
                     resultValue.get().getNumPos());
        assertEquals(outputWithCorrectCounts.get().getDifBtss(),
                     resultValue.get().getDifBtss());
        assertEquals(outputWithCorrectCounts.get().getDifMuns(),
                     resultValue.get().getDifMuns());
        assertEquals(outputWithCorrectCounts.get().getDifStates(),
                     resultValue.get().getDifStates());
        assertEquals("Checking mass center UTM coord X",
                     outputWithCorrectCounts.get().getMasscenterUtmX(),
                     resultValue.get().getMasscenterUtmX(), this.TOLERANCE);
        assertEquals("Checking mass center UTM coord Y",
                     outputWithCorrectCounts.get().getMasscenterUtmY(),
                     resultValue.get().getMasscenterUtmY(), this.TOLERANCE);
        assertEquals("Checking radius",
                     outputWithCorrectCounts.get().getRadius(),
                     resultValue.get().getRadius(), this.TOLERANCE);
        assertEquals("Checking diameter",
                     outputWithCorrectCounts.get().getDiamAreaInf(),
                     resultValue.get().getDiamAreaInf(), this.TOLERANCE);
    }
}

