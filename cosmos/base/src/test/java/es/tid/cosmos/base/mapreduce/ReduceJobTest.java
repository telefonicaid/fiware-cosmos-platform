package es.tid.cosmos.base.mapreduce;

import com.mongodb.hadoop.io.BSONWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import static org.junit.Assert.assertEquals;
import org.junit.Test;

/**
 *
 * @author ximo
 */
public class ReduceJobTest {    
    private static class DummyReducer
        extends Reducer<LongWritable, Text, NullWritable, BSONWritable> {
    }

    @Test
    public void testCreate() throws Exception {
        final String jobName = "ReduceJobName";
        ReduceJob job = ReduceJob.create(
                new Configuration(), jobName,
                TextInputFormat.class,
                DummyReducer.class,
                TextOutputFormat.class);
        assertEquals(job.getJobName(), jobName);
        assertEquals(job.getInputFormatClass(), TextInputFormat.class);
        assertEquals(job.getMapOutputKeyClass(), LongWritable.class);
        assertEquals(job.getMapOutputValueClass(), Text.class);
        assertEquals(job.getOutputKeyClass(), NullWritable.class);
        assertEquals(job.getOutputValueClass(), BSONWritable.class);
        assertEquals(job.getReducerClass(), DummyReducer.class);
        assertEquals(job.getMapperClass(), Mapper.class);
    }
}
