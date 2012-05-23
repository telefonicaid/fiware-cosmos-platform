package es.tid.cosmos.base.mapreduce;

import java.io.IOException;
import java.util.LinkedList;

import org.apache.hadoop.mapreduce.Job;

/**
 * This is a simple extension of LinkedList<Job> that has a waitForCompletion
 * method that will call all jobs in the list in parallel (by submitting them
 * all before waiting for them)
 *
 * @author ximo
 */
public class JobList extends LinkedList<Job> {
    /**
     * This will call submit on all jobs in the list and then call
     * waitForCompletion on each of them. The jobs in the collection must not
     * depend on each other for this method to work correctly.
     *
     * @param verbose
     * @throws IOException
     * @throws InterruptedException
     * @throws ClassNotFoundException
     */
    public void waitForCompletion(boolean verbose)
            throws IOException, InterruptedException, ClassNotFoundException {
        for (Job job : this) {
            job.submit();
        }
        for (Job job : this) {
            job.waitForCompletion(verbose);
        }
    }
}