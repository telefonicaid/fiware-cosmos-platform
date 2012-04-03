package es.tid.bdp.platform.cluster.server;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author sortega
 */
public class ClusterServerTest {
    private ClusterServer instance;

    @Before
    public void setUp() throws Exception {
        this.instance = new ClusterServer();
    }

    @Test
    public void shouldAbsolutizeHdfsPaths() throws Exception {
        assertEquals("hdfs://pshdp01:8011/bdp/path",
                     this.instance.absoluteHdfsPath("/bdp/path"));
        assertEquals("hdfs://absolute/bdp/path",
                     this.instance.absoluteHdfsPath("hdfs://absolute/bdp/path"));
    }
}
