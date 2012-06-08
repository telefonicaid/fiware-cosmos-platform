package es.tid.cosmos.mobility.mivs;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
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
public class IndVarsOutAccReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>,
        NullWritable, Text> reducer;

    @Before
    public void setUp() {
        //this.reducer = new ReduceDriver<LongWritable,
        //        ProtobufWritable<MobData>, NullWritable, Text>(
        //                new IndVarsOutAccReducer());
    }

    @Test
    public void testMissingProfilesFilledWithMinusOnes() {
        LongWritable userNotSeenInFirstMonth = new LongWritable(5512684400L);
        MobVars area = MobVarsUtil.create(1, false, 1, 1, 1, 1,
                                          1000000D, 1000000D, 0.0, 0.0);
        ProtobufWritable<MobData> input = MobDataUtil.createAndWrap(
            MobViMobVarsUtil.create(asList(area)));

        String s = "5512684400|-1|-1|-1|-1|-1|-1|-1|-1|1|1|1|1|1000000.0|"
                + "1000000.0|0.0|0.0";
        Text expected = new Text(s);

        this.reducer
                .withInput(userNotSeenInFirstMonth, asList(input))
                .withOutput(NullWritable.get(), expected)
                .runTest();
    }
}
