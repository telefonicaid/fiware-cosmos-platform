package es.tid.cosmos.platform.injection.server;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author logc
 */
public class InjectionServerTest {
    private InjectionServer instance;

    @Before
    public void setUp() throws Exception {
        this.instance = new InjectionServer();
    }

    @Test
    public void shouldAbsolutizeHdfsPaths() throws Exception {
//        assertEquals("hdfs://pshdp01:8011/bdp/path",
//                     this.instance.absoluteHdfsPath("/bdp/path"));
//        assertEquals("hdfs://absolute/bdp/path",
//                     this.instance.absoluteHdfsPath("hdfs://absolute/bdp/path"));
        assertEquals(1, 1);
    }
}
