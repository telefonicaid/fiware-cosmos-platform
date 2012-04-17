package es.tid.cosmos.platform.cluster.server;

import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;

import es.tid.cosmos.platform.cluster.server.generated.ClusterJobResult;
import es.tid.cosmos.platform.cluster.server.generated.ClusterJobStatus;

/**
 *
 * @author dmicol
 */
class JobStatus {
    private final Future<ClusterJobResult> future;
    
    public JobStatus (Future<ClusterJobResult> future) {
        this.future = future;
    }
    
    public ClusterJobResult getResult() {
        ClusterJobResult result = null;
        if (this.future.isDone()) {
            try {
                result = this.future.get();
            } catch (ExecutionException ex) {
            } catch (InterruptedException ex) {
            } finally {
                if (result == null) {
                    result = new ClusterJobResult();
                }
                result.setStatus(ClusterJobStatus.FAILED);
            }
        } else {
            result = new ClusterJobResult();
            result.setStatus(ClusterJobStatus.RUNNING);
        }
        return result;
    }
}
