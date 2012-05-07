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
public class MapReduceJobTest {
    private static class DummyMapper
        extends Mapper<LongWritable, Text, NullWritable, BSONWritable> {
    }
    
    private static class DummyReducer
        extends Reducer<NullWritable, BSONWritable, Text, Text> {
    }

    @Test
    public void testCreate1() throws Exception {
        final String jobName = "MapReduceJobName";
        MapReduceJob job = MapReduceJob.create(
                new Configuration(), jobName,
                TextInputFormat.class, DummyMapper.class,
                DummyReducer.class, TextOutputFormat.class);
        assertEquals(job.getJobName(), jobName);
        assertEquals(job.getInputFormatClass(), TextInputFormat.class);
        assertEquals(job.getMapOutputKeyClass(), NullWritable.class);
        assertEquals(job.getMapOutputValueClass(), BSONWritable.class);
        assertEquals(job.getOutputKeyClass(), Text.class);
        assertEquals(job.getOutputValueClass(), Text.class);
        assertEquals(job.getReducerClass(), DummyReducer.class);
        assertEquals(job.getMapperClass(), DummyMapper.class);
    }

    @Test
    public void testCreate2() throws Exception {
        final String jobName = "MapReduceJobName";
        final int reduceTasks = 8;
        MapReduceJob job = MapReduceJob.create(
                new Configuration(), jobName,
                TextInputFormat.class, DummyMapper.class,
                DummyReducer.class, reduceTasks,  TextOutputFormat.class);
        assertEquals(job.getJobName(), jobName);
        assertEquals(job.getInputFormatClass(), TextInputFormat.class);
        assertEquals(job.getMapOutputKeyClass(), NullWritable.class);
        assertEquals(job.getMapOutputValueClass(), BSONWritable.class);
        assertEquals(job.getOutputKeyClass(), Text.class);
        assertEquals(job.getOutputValueClass(), Text.class);
        assertEquals(job.getReducerClass(), DummyReducer.class);
        assertEquals(job.getMapperClass(), DummyMapper.class);
        assertEquals(job.getNumReduceTasks(), reduceTasks);
    }
}
