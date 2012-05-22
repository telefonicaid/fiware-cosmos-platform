package es.tid.cosmos.base.mapreduce;

import java.util.LinkedList;
import java.util.List;

/**
 *
 * @author dmicol
 */
public class CosmosJobCollection {
    private List<CosmosJob> jobs;
    
    public CosmosJobCollection() {
        this.jobs = new LinkedList<CosmosJob>();
    }
    
    public void add(CosmosJob job) {
        this.jobs.add(job);
    }
    
    public void waitForCompletion(boolean verbose) throws Exception {
        // Create thread objects for each Cosmos job
        List<CosmosJobThread> threads = new LinkedList<CosmosJobThread>();
        for (CosmosJob job : jobs) {
            threads.add(new CosmosJobThread(job, verbose));
        }
        
        // Start all threads in parallel
        for (CosmosJobThread thread : threads) {
            thread.start();
        }
        
        // Wait for all threads to finish
        for (CosmosJobThread thread : threads) {
            thread.join();
            if (!thread.getSuccessful()) {
                throw new Exception("Failed job");
            }
        }
    }
}
