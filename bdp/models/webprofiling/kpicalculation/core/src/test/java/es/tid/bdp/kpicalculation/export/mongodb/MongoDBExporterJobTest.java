package es.tid.bdp.kpicalculation.export.mongodb;

import java.io.IOException;

import com.mongodb.hadoop.util.MongoConfigUtil;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.bdp.kpicalculation.config.KpiFeature;

/**
 *
 * @author dmicol
 */
public class MongoDBExporterJobTest {
    private MongoDBExporterJob instance;
    
    @Before
    public void setUp() throws IOException {
        this.instance = new MongoDBExporterJob(new Configuration());
    }
    
    @Test
    public void testConfigure() throws IOException {
        Path path = new Path("file:/opt/test");
        String url = "mongodb://localhost:27017/";
        KpiFeature feature = new KpiFeature(
                "PAGE_VIEWS_PROT_VIS_DEV",
                new String[] { "visitorId", "protocol", "device", "date" });
        this.instance.configure(path, url, feature);
        Path[] inputPaths = FileInputFormat.getInputPaths(this.instance);
        assertEquals(1, inputPaths.length);
        assertEquals(path, inputPaths[0]);
        assertEquals(url, MongoConfigUtil.getOutputURI(
                this.instance.getConfiguration()).toString());
    }
}
