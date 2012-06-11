package es.tid.cosmos.mobility.mivs;

import static java.util.Arrays.asList;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobVarsUtil;
import es.tid.cosmos.mobility.data.MobViMobVarsUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobVars;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobViMobVars;

/**
 *
 * @author logc
 */
public class FusionTotalVarsReducerTest {
    private ReduceDriver<LongWritable, MobilityWritable<MobVars>,
            LongWritable, MobilityWritable<MobViMobVars>> reducer;

    @Before
    public void setUp() {
        this.reducer = new ReduceDriver<LongWritable, MobilityWritable<MobVars>,
                LongWritable, MobilityWritable<MobViMobVars>>(
                        new FusionTotalVarsReducer());
    }

    @Test
    public void testJoinsDifferentRows(){
        LongWritable userWithTwoEntries = new LongWritable(5512684400L);

        MobVars area1 = MobVarsUtil.create(1, true, 1, 1, 1, 1, 1000000,
                                           1000000, 0.0, 0.0);
        MobilityWritable<MobVars> row1 = new MobilityWritable<MobVars>(area1);
        MobVars area2 = MobVarsUtil.create(1, true, 2, 2, 2, 2, 6000000,
                                           3000000, 100, 100);
        MobilityWritable<MobVars> row2 = new MobilityWritable<MobVars>(area2);
        MobilityWritable<MobViMobVars> results = new MobilityWritable<MobViMobVars>(
                MobViMobVarsUtil.create(asList(area1, area2)));

        this.reducer
                .withInput(userWithTwoEntries, asList(row1, row2))
                .withOutput(userWithTwoEntries, results)
                .runTest();
    }
}
