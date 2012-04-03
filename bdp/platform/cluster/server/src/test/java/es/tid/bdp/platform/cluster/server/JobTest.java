package es.tid.bdp.platform.cluster.server;

import org.junit.After;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author dmicol
 */
public class JobTest {
    private Job instance;
    
    @Before
    public void setUp() {
        ClusterServerUtil.disallowExitCalls();
    }
    
    @After
    public void tearDown() {
        ClusterServerUtil.allowExitCalls();
    }
    
    @Test
    public void shouldReturnSuccessful() {
        this.instance = new Job(new String[] {}) {
            @Override
            protected void run() {
                // Do not invoke the Hadoop Jar
            }
        };
        
        ClusterJobResult result = this.instance.call();
        assertEquals(ClusterJobStatus.SUCCESSFUL, result.status);
        assertNull(result.reason);
    }

    @Test
    public void shouldReturnFailed() {
        this.instance = new Job(new String[] {}) {
            @Override
            protected void run() {
                // Do not invoke the Hadoop Jar
                throw new RuntimeException("Failed!");
            }
        };
        
        ClusterJobResult result = this.instance.call();
        assertEquals(ClusterJobStatus.FAILED, result.status);
        assertEquals(ClusterErrorCode.RUN_JOB_FAILED, result.reason.errorCode);
        assertNotNull(result.reason.errorMsg);
    }

    @Test
    public void shouldCaptureExitWithSuccess() {
        this.instance = new Job(new String[] {}) {
            @Override
            protected void run() {
                // Do not invoke the Hadoop Jar
                System.exit(0);
            }
        };
        
        ClusterJobResult result = this.instance.call();
        assertEquals(ClusterJobStatus.SUCCESSFUL, result.status);
        assertNull(result.reason);
    }

    @Test
    public void shouldCaptureExitWithFailure() {
        this.instance = new Job(new String[] {}) {
            @Override
            protected void run() {
                // Do not invoke the Hadoop Jar
                System.exit(1);
            }
        };
        
        ClusterJobResult result = this.instance.call();
        assertEquals(ClusterJobStatus.FAILED, result.status);
        assertEquals(ClusterErrorCode.RUN_JOB_FAILED, result.reason.errorCode);
    }
}
