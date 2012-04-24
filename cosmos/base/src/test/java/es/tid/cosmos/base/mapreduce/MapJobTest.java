package es.tid.cosmos.base.mapreduce;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.LongWritable;
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
public class MapJobTest {
    private static class DummyMapper
        extends Mapper<LongWritable, Text, Text, Text> {
    }

    @Test
    public void testCreate() throws Exception {
        final String jobName = "MapperJobName";
        MapJob job = MapJob.create(
                new Configuration(), jobName,
                TextInputFormat.class,
                DummyMapper.class,
                TextOutputFormat.class);
        assertEquals(job.getJobName(), jobName);
        assertEquals(job.getInputFormatClass(), TextInputFormat.class);
        assertEquals(job.getMapOutputKeyClass(), Text.class);
        assertEquals(job.getMapOutputValueClass(), Text.class);
        assertEquals(job.getOutputKeyClass(), Text.class);
        assertEquals(job.getOutputValueClass(), Text.class);
        assertEquals(job.getReducerClass(), Reducer.class);
        assertEquals(job.getMapperClass(), DummyMapper.class);
    }
}
