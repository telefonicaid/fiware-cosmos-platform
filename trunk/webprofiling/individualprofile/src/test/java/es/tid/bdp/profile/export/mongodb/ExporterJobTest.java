package es.tid.bdp.profile.export.mongodb;

import java.io.IOException;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author dmicol
 */
public class ExporterJobTest {
    private ExporterJob instance;
    
    @Before
    public void setUp() throws IOException {
        this.instance = new ExporterJob(new Configuration());
    }
    
    @Test
    public void testConfigure() throws IOException {
        this.instance.configure(new Path("/opt/test1"), "http://localhost");
    }
}
