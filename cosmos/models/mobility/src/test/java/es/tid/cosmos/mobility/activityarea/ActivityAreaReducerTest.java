package es.tid.cosmos.mobility.activityarea;

import java.io.IOException;
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
    private ProtobufWritable<ActivityAreaKey> userWithSingleEntry;
    private ProtobufWritable<ActivityAreaKey> userWithTwoEntries;
    private ProtobufWritable<Cell> firstCell;
    private ProtobufWritable<Cell> secondCell;

    @Before
    public void setUp() {
        this.reducer = new ReduceDriver<
            ProtobufWritable<ActivityAreaKey>, ProtobufWritable<Cell>,
            ProtobufWritable<ActivityAreaKey>, ProtobufWritable<ActivityArea>>(
                    new ActivityAreaReducer());

        int firstPosX = 500000;
        int secondPosX = 700000;
        int firstPosY = 2000000;
        int secondPosY = 4000000;

        this.userWithSingleEntry = ActivityAreaKeyUtil.createAndWrap(
                5512683500L, 1, true);
        this.firstCell = CellUtil.createAndWrap(590379901L, 100L,
                1, 2,
                firstPosX, firstPosY);
        this.userWithTwoEntries = ActivityAreaKeyUtil.createAndWrap(
                5512684400L, 1, true);
        this.secondCell = CellUtil.createAndWrap(591266215L, 100L,
                3, 4,
                secondPosX, secondPosY);
    }

    @Test
    public void testEmitsAllVariables() throws IOException {
        int numPos = 1;
        int difBtss = 1;
        int difMuns = 1;
        int difStates = 1;
        double masscenterUtmX = 500000D;
        double masscenterUtmY = 2000000D;
        double radius = 0.0;
        double diamAreaInf = 0.0;

        ProtobufWritable<ActivityArea> outputWithAllVariables = 
            ActivityAreaUtil.createAndWrap(numPos, difBtss, difMuns,
                    difStates, masscenterUtmX,
                    masscenterUtmY, radius,
                    diamAreaInf);
        List<Pair<ProtobufWritable<ActivityAreaKey>,
             ProtobufWritable<ActivityArea>>> results =
                 this.reducer
                        .withInputKey(this.userWithSingleEntry)
                        .withInputValue(this.firstCell)
                        .run();

        assertEquals(1, results.size());
        ProtobufWritable<ActivityAreaKey> resultKey =
            results.get(0).getFirst();
        assertEquals(this.userWithSingleEntry, resultKey);
        ProtobufWritable<ActivityArea> resultValue =
            results.get(0).getSecond();
        assertEquals(outputWithAllVariables, resultValue);
    }

    @Test
    public void testCountsMakeSense() throws IOException {
        int numPos = 2;
        int difBtss = 2;
        int difMuns = 2;
        int difStates = 2;
        double masscenterUtmX = (double)500000;
        double masscenterUtmY = (double)2000000;
        double radius = 0.0;
        double diamAreaInf = 0.0;

        ActivityArea outputWithCorrectCounts = 
            ActivityAreaUtil.create(numPos, difBtss, difMuns, difStates,
                    masscenterUtmX, masscenterUtmY, radius,
                    diamAreaInf);
        List<Pair<ProtobufWritable<ActivityAreaKey>,
            ProtobufWritable<ActivityArea>>> results =
                this.reducer
                        .withInputKey(this.userWithTwoEntries)
                        .withInputValue(this.firstCell)
                        .withInputValue(this.secondCell)
                        .run();

        assertEquals(1, results.size());
        ProtobufWritable<ActivityAreaKey> resultKey =
            results.get(0).getFirst();
        assertEquals(this.userWithTwoEntries, resultKey);
        ProtobufWritable<ActivityArea> resultValue =
            results.get(0).getSecond();
        resultValue.setConverter(ActivityArea.class);
        assertEquals(outputWithCorrectCounts, resultValue.get());
    }
}

