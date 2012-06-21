package es.tid.cosmos.samples.wordcount;

import com.mongodb.hadoop.util.MongoConfigUtil;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

/**
 * @author sortega
 */
public class MongoDBExporterJobTest extends JobTest {
    private MongoDBExporterJob instance;

    @Before
    public void setUp() throws Exception {
        Configuration config = new Configuration();
        config.set("foo", "bar");
        this.instance = new MongoDBExporterJob(config);
    }

    @Test
    public void shouldHaveInitialConfig() throws Exception {
        assertEquals("bar", this.instance.getConfiguration().get("foo"));
    }

    @Test
    public void shouldBeConfigured() throws Exception {
        final Path inputPath = new Path("file:/tmp/text");
        final String outputUrl = "mongodb://host/db";
        this.instance.configure(inputPath, outputUrl);

        assertArrayEquals(new Path[] { inputPath },
                          FileInputFormat.getInputPaths(this.instance));
        assertEquals(outputUrl, MongoConfigUtil.getOutputURI(
                this.instance.getConfiguration()).toString());

        assertMRChain(this.instance, DEFAULT_MAPPER,
                                     MongoDBExporterReducer.class);
    }
}
