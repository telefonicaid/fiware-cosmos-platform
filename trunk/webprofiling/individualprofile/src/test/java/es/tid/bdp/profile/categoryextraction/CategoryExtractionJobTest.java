package es.tid.bdp.profile.categoryextraction;

import java.io.IOException;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author dmicol
 */
public class CategoryExtractionJobTest {
    private CategoryExtractionJob instance;
    
    @Before
    public void setUp() throws IOException {
        this.instance = new CategoryExtractionJob(new Configuration());
    }
    
    @Test
    public void testConfigureTextInput() {
        this.instance.configureTextInput();
    }

    @Test
    public void testConfigureProtobufInput() {
        this.instance.configureProtobufInput();
    }

    @Test
    public void testConfigurePaths() throws IOException {
        this.instance.configurePaths(new Path("/opt/test1"),
                                     new Path("/opt/test2"));
    }
}
