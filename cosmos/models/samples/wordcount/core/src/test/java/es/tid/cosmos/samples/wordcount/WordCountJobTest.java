package es.tid.bdp.samples.wordcount;

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
public class WordCountJobTest {
    private WordCountJob instance;

    @Before
    public void setUp() throws IOException {
        this.instance = new WordCountJob(new Configuration());
    }

    @Test
    public void testConfigurePaths() throws IOException {
        Path path1 = new Path("file:/opt/test1");
        Path path2 = new Path("file:/opt/test2");
        this.instance.configure(path1, path2);
        Path[] inputPaths = FileInputFormat.getInputPaths(this.instance);
        assertEquals(1, inputPaths.length);
        assertEquals(path1, inputPaths[0]);
        assertEquals(path2, FileOutputFormat.getOutputPath(this.instance));
    }
}
