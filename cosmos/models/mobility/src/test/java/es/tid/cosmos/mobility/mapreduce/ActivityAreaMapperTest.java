package es.tid.cosmos.mobility.mapreduce;

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
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

public class ActivityAreaMapperTest {
    private MapDriver<ProtobufWritable<ActivityAreaKey>, ProtobufWritable<Cell>,
            ProtobufWritable<ActivityAreaKey>, ProtobufWritable<Cell>>
            mapper;
    private ProtobufWritable<ActivityAreaKey> userWithSingleEntry;
    private ProtobufWritable<Cell> firstCell;

    @Before
    public void setUp() {
        this.mapper = new MapDriver<ProtobufWritable<ActivityAreaKey>,
                ProtobufWritable<Cell>, ProtobufWritable<ActivityAreaKey>,
                ProtobufWritable<Cell>>(new ActivityAreaMapper());

        long firstUserId = 5512683500L;
        int month = 1;
        boolean isWorkDay = true;

        long firstCellId = 590379901L;
        long firstPlaceId = 100L;
        int firstGeoLoc1 = 1;
        int firstGeoLoc2 = 2;
        int firstPosX = 500000;
        int firstPosY = 2000000;

        this.userWithSingleEntry = ActivityAreaKeyUtil.createAndWrap(
                firstUserId, month, isWorkDay);
        this.firstCell = CellUtil.createAndWrap(firstCellId, firstPlaceId,
                                                firstGeoLoc1, firstGeoLoc2,
                                                firstPosX, firstPosY);
    }

    @Test
    public void isIdentity() throws IOException {
        this.mapper
            .withInput(this.userWithSingleEntry, this.firstCell)
            .withOutput(this.userWithSingleEntry, this.firstCell)
            .runTest();
    }
}
