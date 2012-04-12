package es.tid.cosmos.mobility.parsing;

import es.tid.cosmos.mobility.jobs.ParseCdrsJob;
import es.tid.cosmos.mobility.jobs.ParseCdrsJob;
import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author dmicol
 */
public class ParseCdrsJobTest {
    private ParseCdrsJob instance;
    
    @Before
    public void setUp() throws IOException {
        this.instance = new ParseCdrsJob(new Configuration());
    }

    @Test
    public void testConfigure() throws IOException {
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
