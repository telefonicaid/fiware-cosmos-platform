package es.tid.cosmos.mobility.mivs;

import java.io.IOException;
import java.io.InputStream;
import static java.util.Arrays.asList;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobVarsUtil;
import es.tid.cosmos.mobility.data.MobViMobVarsUtil;
import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.Config;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobVars;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobViMobVars;

/**
 *
 * @author logc
 */
public class IndVarsOutReducerTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<MobViMobVars>,
            NullWritable, Text> reducer;

    @Before
    public void setUp() throws IOException {
        this.reducer = new ReduceDriver<LongWritable,
                TypedProtobufWritable<MobViMobVars>, NullWritable, Text>(
                        new IndVarsOutReducer());
        InputStream configInput = Config.class.getResource(
                "/mobility.properties").openStream();
        this.reducer.setConfiguration(Config.load(configInput,
                this.reducer.getConfiguration()));
    }

    @Test
    public void testMissingProfilesFilledWithMinusOnes() {
        LongWritable userNotSeenInFirstMonth = new LongWritable(5512684400L);
        MobVars area = MobVarsUtil.create(1, false, 1, 1, 1, 1,
                                          1000000D, 1000000D, 0.0, 0.0);
        TypedProtobufWritable<MobViMobVars> input = new TypedProtobufWritable<MobViMobVars>(
            MobViMobVarsUtil.create(asList(area)));

        String s = "5512684400|1|-1|-1|-1|-1|-1|-1|-1|-1|"
                 + "1|1|1|1|1000000.0|1000000.0|0.0|0.0";
        Text expected = new Text(s);

        String extra1 = "5512684400|2|-1|-1|-1|-1|-1|-1|-1|"
                      + "-1|-1|-1|-1|-1|-1|-1|-1|-1";
        Text expectedExtra1 = new Text(extra1);

        String extra2 = "5512684400|3|-1|-1|-1|-1|-1|-1|-1|"
                      + "-1|-1|-1|-1|-1|-1|-1|-1|-1";
        Text expectedExtra2 = new Text(extra2);

        String extra3 = "5512684400|4|-1|-1|-1|-1|-1|-1|-1|"
                      + "-1|-1|-1|-1|-1|-1|-1|-1|-1";
        Text expectedExtra3 = new Text(extra3);

        String extra4 = "5512684400|5|-1|-1|-1|-1|-1|-1|-1|"
                      + "-1|-1|-1|-1|-1|-1|-1|-1|-1";
        Text expectedExtra4 = new Text(extra4);

        String extra5 = "5512684400|6|-1|-1|-1|-1|-1|-1|-1|"
                      + "-1|-1|-1|-1|-1|-1|-1|-1|-1";
        Text expectedExtra5 = new Text(extra5);

        this.reducer
                .withInputKey(userNotSeenInFirstMonth)
                .withInputValues(asList(input))
                .withOutput(NullWritable.get(), expected)
                .withOutput(NullWritable.get(), expectedExtra1)
                .withOutput(NullWritable.get(), expectedExtra2)
                .withOutput(NullWritable.get(), expectedExtra3)
                .withOutput(NullWritable.get(), expectedExtra4)
                .withOutput(NullWritable.get(), expectedExtra5)
                .runTest();
    }
}
