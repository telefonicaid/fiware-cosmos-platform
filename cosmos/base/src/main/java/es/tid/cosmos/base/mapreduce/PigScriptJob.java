package es.tid.cosmos.base.mapreduce;

import java.io.IOException;
import java.net.URL;
import java.util.Map;

import org.apache.pig.ExecType;
import org.apache.pig.PigServer;
import org.apache.pig.backend.executionengine.ExecException;

/**
 * A cosmos workflow that executes an Apache Pig script as a independent job.
 *
 * @author apv
 */
public class PigScriptJob implements CosmosWorkflow {

    private final PigServer pigServer;
    private final URL scriptLocation;
    private final Map<String, String> scriptParams;
    private final WorkflowList dependencies;
    private volatile ExceptionedThread runner;

    public PigScriptJob(URL scriptLocation,
                        Map<String, String> scriptParams) {
        this(getDefaultPigServer(), scriptLocation, scriptParams);
    }

    public PigScriptJob(PigServer pigServer, URL scriptLocation,
                        Map<String, String> scriptParams) {
        this.pigServer = pigServer;
        this.scriptLocation = scriptLocation;
        this.scriptParams = scriptParams;
        this.dependencies = new WorkflowList();
        this.runner = null;
    }


    @Override
    public boolean waitForCompletion(boolean verbose)
            throws IOException, InterruptedException, ClassNotFoundException {
        this.launchRunner();
        this.runner.join();
        return true;
    }

    @Override
    public void submit() {
        this.launchRunner();
    }

    @Override
    public void addDependentWorkflow(CosmosWorkflow workflow) {
        if (this.runner != null) {
            throw new IllegalStateException("cannot add dependent workflow " +
                    "once pig script is submitted");
        }
        this.dependencies.add(workflow);
    }

    private void launchRunner() {
        if (this.runner != null) {
            return;
        }

        this.runner = new ExceptionedThread() {
            @Override
            public void run() {
                try {
                    PigScriptJob.this.dependencies.submit();
                    PigScriptJob.this.dependencies.waitForCompletion(true);
                    PigScriptJob.this.submitScript();
                } catch (Exception e) {
                    this.setException(e);
                }
            }
        };
        this.runner.start();
    }

    private void submitScript() throws IOException {
        this.pigServer.registerScript(
                this.scriptLocation.openStream(), this.scriptParams);
        this.pigServer.setBatchOn();
        this.pigServer.executeBatch();
    }

    private static PigServer getDefaultPigServer() {
        try {
            return new PigServer(ExecType.MAPREDUCE);
        } catch (ExecException e) {
            throw new IllegalStateException("cannot instantiate a pig server " +
                    "in MAPREDUCE execution mode", e);
        }
    }

}
