package es.tid.cosmos.mobility.clientlabelling;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.assertEquals;

/**
 *
 * @author dmicol
 */
public class VectorFuseNodeDaygroupJobTest {
    private VectorFuseNodeDaygroupJob instance;
    
    @Before
    public void setUp() throws IOException {
        this.instance = new VectorFuseNodeDaygroupJob(new Configuration());
    }

    @Test
    public void testConfigure() throws Exception {
        Path input = new Path("hdfs://home/hdfs/input");
        Path output = new Path("hdfs://home/hdfs/output");
        this.instance.configure(input, output);
        
        Path[] inputPaths = FileInputFormat.getInputPaths(this.instance);
        assertEquals(1, inputPaths.length);
        assertEquals(input, inputPaths[0]);
        Path outputPath = FileOutputFormat.getOutputPath(this.instance);
        assertEquals(output, outputPath);
    }
}
