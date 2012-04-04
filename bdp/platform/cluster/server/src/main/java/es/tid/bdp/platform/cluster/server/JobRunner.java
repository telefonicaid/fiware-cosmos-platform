package es.tid.bdp.platform.cluster.server;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

/**
 *
 * @author dmicol
 */
public class JobRunner {
    private static final int MAX_THREADS = 10;
    
    private ExecutorService threadPool;
    private Map<String, Future<ClusterJobResult>> results;

    public JobRunner() {
        this.threadPool = Executors.newFixedThreadPool(MAX_THREADS);
        this.results = new HashMap<String, Future<ClusterJobResult>>();
    }
    
    public synchronized void startNewThread(String id, String[] args) {
        Future<ClusterJobResult> status = this.threadPool.submit(
                new Job(args));
        this.results.put(id, status);
    }

    public ClusterJobResult getResult(String id)
            throws ExecutionException, InterruptedException, TransferException {
        ClusterJobResult result;
        Future<ClusterJobResult> resultFuture = this.results.get(id);
        if (resultFuture == null) {
            throw new TransferException(ClusterErrorCode.INVALID_JOB_ID, null);
        }

        if (resultFuture.isDone()) {
            result = resultFuture.get();
        } else {
            result = new ClusterJobResult();
            result.setStatus(ClusterJobStatus.RUNNING);
        }
        return result;
    }
}