package es.tid.cosmos.mobility.activityarea;

import java.util.List;
import java.util.ArrayList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

import es.tid.cosmos.mobility.data.ActivityAreaUtil;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityArea;

/**
 *
 * @author losa
 */
public class FusionTotalVarsReducerTest {
    private ReduceDriver<
        LongWritable,
        ProtobufWritable<ActivityArea>,
        LongWritable,
        List<ProtobufWritable<ActivityArea>>> 
        reducer;

    @Before
    public void setUp() {
        this.reducer = new ReduceDriver<
            LongWritable,
            ProtobufWritable<ActivityArea>,
            LongWritable,
            List<ProtobufWritable<ActivityArea>>>(
                    new FusionTotalVarsReducer());
    }

    @Test
    public void testJoinsDifferentRows(){
        LongWritable userWithTwoEntries = new LongWritable(5512684400L);

        ProtobufWritable<ActivityArea> row1 = 
            ActivityAreaUtil.createAndWrap(1, 1, 1, 1, 1000000, 1000000,
                                           0.0, 0.0);
        ProtobufWritable<ActivityArea> row2 = 
            ActivityAreaUtil.createAndWrap(2, 2, 2, 2, 6000000, 3000000,
                                           100, 100);
    }
}
