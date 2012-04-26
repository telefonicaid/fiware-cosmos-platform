package es.tid.cosmos.base.mapreduce;

import java.io.IOException;
import java.util.EnumSet;
import java.util.List;

/**
 *
 * @author ximo
 */
public interface Runnable {
    /**
     * This method always returns true, and it must throw a
     * JobExecutionException if a failure occurs during job execution. It must
     * behave exactly like calling waitForCompletion(verbose,
     * EnumSet.noneOf(CleanupOptions.class))
     *
     * @return Always returns true
     * @throws IOException
     * @throws InterruptedException
     * @throws ClassNotFoundException
     * @throws JobExecutionException
     */
    boolean waitForCompletion(boolean verbose, EnumSet<CleanupOptions> options)
            throws IOException, InterruptedException, ClassNotFoundException;

    /**
     * This method is here to mimic the waitForCompletion method in Hadoop's
     * jobs. It must always returns true, and it must throw a
     * JobExecutionException if a failure occurs during job execution. It must
     * behave exactly like calling waitForCompletion(verbose,
     * EnumSet.noneOf(CleanupOptions.class))
     *
     * @param verbose
     * @return Always returns true
     * @throws IOException
     * @throws InterruptedException
     * @throws ClassNotFoundException
     * @throws JobExecutionException
     */
    boolean waitForCompletion(boolean verbose)
            throws IOException, InterruptedException, ClassNotFoundException;

    List<CosmosJob> getJobs();
}
