package es.tid.cosmos.samples.wordcount;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

/**
 * @author sortega
 */
public class WordCountJobTest extends JobTest {
    private WordCountJob instance;

    @Before
    public void setUp() throws Exception {
        Configuration config = new Configuration();
        config.set("foo", "bar");
        this.instance = new WordCountJob(config);
    }

    @Test
    public void shouldHaveInitialConfig() throws Exception {
        assertEquals("bar", this.instance.getConfiguration().get("foo"));
    }

    @Test
    public void shouldBeConfigured() throws Exception {
        Path inputPath = new Path("file:/tmp/text");
        Path outputPath = new Path("file:/tmp/out");
        this.instance.configure(inputPath, outputPath);

        assertArrayEquals(new Path[] { inputPath },
                          FileInputFormat.getInputPaths(this.instance));
        assertEquals(outputPath,
                     FileOutputFormat.getOutputPath(this.instance));

        assertMRChain(this.instance, WordCountMapper.class,
                                     WordCountReducer.class);
    }
}
