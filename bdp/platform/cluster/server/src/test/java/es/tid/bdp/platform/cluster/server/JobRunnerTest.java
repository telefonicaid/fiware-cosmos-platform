package es.tid.bdp.platform.cluster.server;

import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author dmicol
 */
public class JobRunnerTest {
    private JobRunner instance;

    @Before
    public void setUp() {
        this.instance = new JobRunner();
    }

    @Test(expected = TransferException.class)
    public void testInvalidJobId() throws TransferException {
        this.instance.getResult("job423");
    }
}
