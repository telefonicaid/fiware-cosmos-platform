package es.tid.cosmos.mobility.mivs;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobVarsUtil;
import es.tid.cosmos.mobility.data.MobViMobVarsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobVars;

/**
 *
 * @author logc
 */
public class FusionTotalVarsReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>, LongWritable,
           ProtobufWritable<MobData>> reducer;

    @Before
    public void setUp() {
        //this.reducer = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
        //        LongWritable, ProtobufWritable<MobData>>(
        //                new FusionTotalVarsReducer());
    }

    @Test
    public void testJoinsDifferentRows(){
        LongWritable userWithTwoEntries = new LongWritable(5512684400L);

        MobVars area1 = MobVarsUtil.create(1, true, 1, 1, 1, 1, 1000000,
                                           1000000, 0.0, 0.0);
        ProtobufWritable<MobData> row1 = MobDataUtil.createAndWrap(area1);
        MobVars area2 = MobVarsUtil.create(1, true, 2, 2, 2, 2, 6000000,
                                           3000000, 100, 100);
        ProtobufWritable<MobData> row2 = MobDataUtil.createAndWrap(area2);
        ProtobufWritable<MobData> results = MobDataUtil.createAndWrap(
                MobViMobVarsUtil.create(asList(area1, area2)));

        this.reducer
                .withInput(userWithTwoEntries, asList(row1, row2))
                .withOutput(userWithTwoEntries, results)
                .runTest();
    }
}
