package es.tid.cosmos.platform.cluster.server;

import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;
import org.junit.Test;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

import es.tid.cosmos.platform.cluster.server.generated.ClusterJobResult;
import es.tid.cosmos.platform.cluster.server.generated.ClusterJobStatus;

/**
 *
 * @author dmicol
 */
public class JobStatusTest {
    private JobStatus instance;
    
    @Test
    public void testRunningJob() {
        Future future = mock(Future.class);
        when(future.isDone()).thenReturn(false);
        
        this.instance = new JobStatus(future);
        ClusterJobResult result = this.instance.getResult();
        assertEquals(ClusterJobStatus.RUNNING, result.status);
        assertNull(result.reason);
    }
    
    @Test
    public void testFinishedJob() throws ExecutionException,
                                         InterruptedException {
        Future future = mock(Future.class);
        when(future.isDone()).thenReturn(true);
        
        this.instance = new JobStatus(future);
        ClusterJobResult result = new ClusterJobResult();
        when(future.get()).thenReturn(result);
        assertEquals(result, this.instance.getResult());
    }
    
    @Test
    public void testExecutionException() throws ExecutionException,
                                                InterruptedException {
        Future future = mock(Future.class);
        when(future.isDone()).thenReturn(true);
        
        this.instance = new JobStatus(future);
        when(future.get()).thenThrow(new ExecutionException(null, null));
        ClusterJobResult result = this.instance.getResult();
        assertEquals(ClusterJobStatus.FAILED, result.status);
        assertNull(result.reason);
    }

    @Test
    public void testInterruptedException() throws ExecutionException,
                                                  InterruptedException {
        Future future = mock(Future.class);
        when(future.isDone()).thenReturn(true);
        
        this.instance = new JobStatus(future);
        when(future.get()).thenThrow(new InterruptedException());
        ClusterJobResult result = this.instance.getResult();
        assertEquals(ClusterJobStatus.FAILED, result.status);
        assertNull(result.reason);
    }
}
