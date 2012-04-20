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

import es.tid.cosmos.mobility.data.MobVarsUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobVars;
import es.tid.cosmos.mobility.data.MobProtocol.MobViMobVars;
import es.tid.cosmos.mobility.data.MobViMobVarsUtil;

/**
 *
 * @author losa
 */
public class FusionTotalVarsReducerTest {
    private ReduceDriver<
        LongWritable,
        ProtobufWritable<MobVars>,
        LongWritable,
        ProtobufWritable<MobViMobVars>>
        reducer;

    @Before
    public void setUp() {
        this.reducer = new ReduceDriver<
            LongWritable,
            ProtobufWritable<MobVars>,
            LongWritable,
            ProtobufWritable<MobViMobVars>>(
                    new FusionTotalVarsReducer());
    }

    @Test
    public void testJoinsDifferentRows(){
        LongWritable userWithTwoEntries = new LongWritable(5512684400L);

        MobVars area1 =
            MobVarsUtil.create(1, true, 1, 1, 1, 1, 1000000, 1000000,
                                    0.0, 0.0);
        ProtobufWritable<MobVars> row1 = MobVarsUtil.wrap(area1);

        MobVars area2 =
            MobVarsUtil.create(1, true, 2, 2, 2, 2, 6000000, 3000000,
                                    100, 100);
        ProtobufWritable<MobVars> row2 = MobVarsUtil.wrap(area2);

        ProtobufWritable<MobViMobVars> results =
            MobViMobVarsUtil.createAndWrap(asList(area1, area2));

        this.reducer
            .withInputKey(userWithTwoEntries)
            .withInputValues(asList(row1, row2))
            .withOutput(userWithTwoEntries, results)
            .runTest();
    }
}
