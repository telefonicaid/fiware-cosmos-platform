package es.tid.bdp.platform.cluster.server;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 *
 * @author dmicol
 */
public class JobRunner {
    private static final int MAX_THREADS = 10;
    
    private ExecutorService threadPool;
    private Map<String, JobStatus> results;

    public JobRunner() {
        this.threadPool = Executors.newFixedThreadPool(MAX_THREADS);
        this.results = new HashMap<String, JobStatus>();
    }
    
    public synchronized void startNewThread(String id, Job job) {
        JobStatus status = new JobStatus(this.threadPool.submit(job));
        this.results.put(id, status);
    }

    public ClusterJobResult getResult(String id) throws TransferException {
        JobStatus status = this.results.get(id);
        if (status == null) {
            throw new TransferException(ClusterErrorCode.INVALID_JOB_ID, null);
        }
        return status.getResult();
    }
}