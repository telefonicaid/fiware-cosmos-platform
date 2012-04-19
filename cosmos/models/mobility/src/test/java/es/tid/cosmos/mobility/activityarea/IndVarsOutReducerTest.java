package es.tid.cosmos.mobility.activityarea;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

import es.tid.cosmos.mobility.data.MobProtocol.RepeatedActivityAreas;

/**
 *
 * @author losa
 */
public class IndVarsOutReducerTest {
    private ReduceDriver<
        LongWritable,  ProtobufWritable<RepeatedActivityAreas>,
        NullWritable,  Text>
        reducer;

    @Before
    public void setUp() {
        this.reducer = new ReduceDriver<
            LongWritable,  ProtobufWritable<RepeatedActivityAreas>,
            NullWritable,  Text>
                (new IndVarsOutReducer());
    }

    @Test
    public void testMissingMonthsFilledWithMinusOnes() {
    }
}
