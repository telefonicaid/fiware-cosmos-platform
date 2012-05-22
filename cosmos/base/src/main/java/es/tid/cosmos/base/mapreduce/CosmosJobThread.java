package es.tid.cosmos.base.mapreduce;

import java.io.IOException;

import es.tid.cosmos.base.util.Logger;

/**
 *
 * @author dmicol
 */
public class CosmosJobThread extends Thread {
    private final CosmosJob job;
    private final boolean verbose;
    private boolean successful;
    
    public CosmosJobThread(CosmosJob job, boolean verbose) {
        this.job = job;
        this.verbose = verbose;
        this.successful = false;
    }
    
    public boolean getSuccessful() {
        return this.successful;
    }
    
    @Override
    public void run() {
        try {
            this.job.waitForCompletion(this.verbose);
            this.successful = true;
        } catch (Exception ex) {
            Logger.get(CosmosJobThread.class).fatal(ex);
        }
    }
}
