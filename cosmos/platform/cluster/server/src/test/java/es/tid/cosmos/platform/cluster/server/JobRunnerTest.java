package es.tid.cosmos.platform.cluster.server;

import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;
import static org.mockito.Mockito.doNothing;
import static org.mockito.Mockito.mock;

import es.tid.cosmos.platform.cluster.server.generated.TransferException;

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

    @Test
    public void testValidJobId() throws TransferException {
        Job job = mock(Job.class);
        doNothing().when(job).run();
        this.instance.startNewThread("job1", job);
        assertNotNull(this.instance.getResult("job1"));
    }
    
    @Test(expected = TransferException.class)
    public void testInvalidJobId() throws TransferException {
        Job job = mock(Job.class);
        doNothing().when(job).run();
        this.instance.startNewThread("job1", job);
        this.instance.getResult("job234");
    }
}
