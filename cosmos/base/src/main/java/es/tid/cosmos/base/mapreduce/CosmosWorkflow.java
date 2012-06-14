package es.tid.cosmos.base.mapreduce;

import java.io.IOException;

/**
 *
 * @author ximo
 */
public interface CosmosWorkflow {
    public boolean waitForCompletion(boolean verbose)
            throws IOException, InterruptedException, ClassNotFoundException;
    public void submit();
    public void addDependentWorkflow(CosmosWorkflow job);
}
