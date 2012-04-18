package es.tid.cosmos.mobility.activityarea;

import java.util.List;
import java.util.ArrayList;
import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

import es.tid.cosmos.mobility.data.ActivityAreaUtil;
import es.tid.cosmos.mobility.data.RepeatedActivityAreasUtil;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityArea;
import es.tid.cosmos.mobility.data.MobProtocol.RepeatedActivityAreas;

/**
 *
 * @author losa
 */
public class FusionTotalVarsReducerTest {
    private ReduceDriver<
        LongWritable,
        ProtobufWritable<ActivityArea>,
        LongWritable,
        ProtobufWritable<RepeatedActivityAreas>>
        reducer;

    @Before
    public void setUp() {
        this.reducer = new ReduceDriver<
            LongWritable,
            ProtobufWritable<ActivityArea>,
            LongWritable,
            ProtobufWritable<RepeatedActivityAreas>>(
                    new FusionTotalVarsReducer());
    }

    @Test
    public void testJoinsDifferentRows(){
        LongWritable userWithTwoEntries = new LongWritable(5512684400L);

        ActivityArea area1 =
            ActivityAreaUtil.create(1, 1, 1, 1, 1000000, 1000000,
                                           0.0, 0.0);
        ProtobufWritable<ActivityArea> row1 = ActivityAreaUtil.wrap(area1);

        ActivityArea area2 =
            ActivityAreaUtil.create(2, 2, 2, 2, 6000000, 3000000, 100, 100);
        ProtobufWritable<ActivityArea> row2 = ActivityAreaUtil.wrap(area2);

        ProtobufWritable<RepeatedActivityAreas> results =
            RepeatedActivityAreasUtil.createAndWrap(asList(area1, area2));

        this.reducer
            .withInputKey(userWithTwoEntries)
            .withInputValues(asList(row1, row2))
            .withOutput(userWithTwoEntries, results)
            .runTest();
    }
}
