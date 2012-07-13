package es.tid.cosmos.base.mapreduce;

/**
 *
 * @author ximo
 */
public class JobExecutionException extends RuntimeException {
    JobExecutionException(String message) {
        super(message);
    }
    
    JobExecutionException(String message, Throwable cause) {
        super(message, cause);
    }
}
