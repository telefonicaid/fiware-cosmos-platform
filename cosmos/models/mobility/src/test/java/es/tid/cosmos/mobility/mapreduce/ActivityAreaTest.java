package es.tid.cosmos.mobility.mapreduce;

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.MapReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import org.junit.Before;
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

public class ActivityAreaTest {
    private MapReduceDriver<
            ProtobufWritable<ActivityAreaKey>, ProtobufWritable<Cell>,
            ProtobufWritable<ActivityAreaKey>, ProtobufWritable<ActivityArea>,
            ProtobufWritable<ActivityAreaKey>, ProtobufWritable<ActivityArea>> driver;
    private ProtobufWritable<ActivityAreaKey> userWithSingleEntry;
    private ProtobufWritable<ActivityAreaKey> userWithTwoEntries;
    private ProtobufWritable<Cell> firstCell;
    private ProtobufWritable<Cell> secondCell;

    @Before
    public void setUp() {
        this.driver = new MapReduceDriver<
                ProtobufWritable<ActivityAreaKey>, ProtobufWritable<Cell>,
                ProtobufWritable<ActivityAreaKey>, ProtobufWritable<ActivityArea>,
                ProtobufWritable<ActivityAreaKey>, ProtobufWritable<ActivityArea>>
                (new ActivityAreaMapper(), new ActivityAreaReducer());

        long firstUserId = 5512683500L;
        long secondUserId = 5512684400L;
        int month = 1;
        boolean isWorkDay = true;

        long firstCellId = 590379901L;
        long secondCellId = 591266215L;
        long firstPlaceId = 100L;
        long secondPlaceId = 100L;
        int firstGeoLoc1 = 1;
        int secondGeoLoc1 = 3;
        int firstGeoLoc2 = 2;
        int secondGeoLoc2 = 4;
        int firstPosX = 500000;
        int secondPosX = 700000;
        int firstPosY = 2000000;
        int secondPosY = 4000000;


        this.userWithSingleEntry = ActivityAreaKeyUtil.createAndWrap(
                firstUserId, month, isWorkDay);
        this.userWithSingleEntry.setConverter(ActivityAreaKey.class);

        this.firstCell = CellUtil.createAndWrap(firstCellId, firstPlaceId,
                                                firstGeoLoc1, firstGeoLoc2,
                                                firstPosX, firstPosY);
        this.firstCell.setConverter(Cell.class);

        this.userWithTwoEntries = ActivityAreaKeyUtil.createAndWrap(
                secondUserId, month, isWorkDay);
        this.userWithTwoEntries.setConverter(ActivityAreaKey.class);

        this.secondCell = CellUtil.createAndWrap(secondCellId, secondPlaceId,
                                                 secondGeoLoc1, secondGeoLoc2,
                                                 secondPosX, secondPosY);
        this.secondCell.setConverter(Cell.class);
    }

    @Test
    public void emitsAllVariables() throws IOException {
        int numPos = 1;
        int difBtss = 1;
        int difMuns = 1;
        int difStates = 1;
        double masscenterUtmX = (double)500000;
        double masscenterUtmY = (double)2000000;
        double radius = 0.0;
        double diamAreaInf = 0.0;

        ProtobufWritable<ActivityArea> outputWithAllVariables = 
                ActivityAreaUtil.createAndWrap(numPos, difBtss, difMuns,
                                               difStates, masscenterUtmX,
                                               masscenterUtmY, radius,
                                               diamAreaInf);
        outputWithAllVariables.setConverter(ActivityArea.class);

        List<Pair<ProtobufWritable<ActivityAreaKey>,
                  ProtobufWritable<ActivityArea>>> results = this.driver
                        .withInput(this.userWithSingleEntry, this.firstCell)
                        .run();

        assertEquals(1, results.size());
        //                .withOutput(this.userWithSingleEntry, outputWithAllVariables)
    }

    @Test
    public void countsMakeSense() throws IOException {
        int numPos = 2;
        int difBtss = 2;
        int difMuns = 2;
        int difStates = 2;
        double masscenterUtmX = (double)500000;
        double masscenterUtmY = (double)2000000;
        double radius = 0.0;
        double diamAreaInf = 0.0;

        ProtobufWritable<ActivityArea> outputWithCorrectCounts = 
                ActivityAreaUtil.createAndWrap(numPos, difBtss, difMuns,
                                               difStates, masscenterUtmX,
                                               masscenterUtmY, radius,
                                               diamAreaInf);
        outputWithCorrectCounts.setConverter(ActivityArea.class);

        List<Pair<ProtobufWritable<ActivityAreaKey>,
                  ProtobufWritable<ActivityArea>>> results = this.driver
                .withInput(this.userWithTwoEntries, this.firstCell)
                .withInput(this.userWithTwoEntries, this.secondCell)
                .run();

        assertEquals(1, results.size());
                //.withOutput(this.userWithTwoEntries, outputWithCorrectCounts)
        
    }
}

