package es.tid.cosmos.base.mapreduce;

import java.util.EnumSet;
import java.util.List;

/**
 *
 * @author ximo
 */
public interface Runnable {
    void waitForCompletion(EnumSet<CleanupOptions> options) throws Exception;    
    List<CosmosJob> getJobs();
}
