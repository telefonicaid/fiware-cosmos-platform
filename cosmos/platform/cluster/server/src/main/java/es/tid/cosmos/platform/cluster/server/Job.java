package es.tid.cosmos.platform.cluster.server;

import java.lang.reflect.UndeclaredThrowableException;
import java.util.concurrent.Callable;

import org.apache.hadoop.util.RunJar;

import es.tid.cosmos.platform.cluster.server.generated.*;

/**
 *
 * @author dmicol
 */
public class Job implements Callable<ClusterJobResult> {
    private String[] args;

    public Job(String[] args) {
        this.args = args.clone();
    }

    @Override
    public ClusterJobResult call() {
        ClusterJobResult result = new ClusterJobResult();
        try {
            this.run();
            result.setStatus(ClusterJobStatus.SUCCESSFUL);
        } catch (ClusterServerUtil.ExitWithSuccessCodeException ex) {
            result.setStatus(ClusterJobStatus.SUCCESSFUL);
        } catch (ClusterServerUtil.ExitWithFailureCodeException ex) {
            result.setStatus(ClusterJobStatus.FAILED);
            result.setReason(new TransferException(
                    ClusterErrorCode.RUN_JOB_FAILED, "Unknown error"));
        } catch (Exception ex) {
            result.setStatus(ClusterJobStatus.FAILED);
            result.setReason(new TransferException(
                    ClusterErrorCode.RUN_JOB_FAILED,
                    ClusterServerUtil.getFullExceptionInformation(ex)));
        }
        return result;
    }

    protected void run() {
        try {
            RunJar.main(this.args);
        } catch (Throwable ex) {
            throw new UndeclaredThrowableException(ex);
        }
    }
}