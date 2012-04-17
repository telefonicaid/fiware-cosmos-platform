package es.tid.cosmos.platform.cluster.server;

import java.lang.reflect.UndeclaredThrowableException;

import org.junit.After;
import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.platform.cluster.server.generated.ClusterErrorCode;
import es.tid.cosmos.platform.cluster.server.generated.ClusterJobResult;
import es.tid.cosmos.platform.cluster.server.generated.ClusterJobStatus;

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
                throw new UndeclaredThrowableException(null);
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
