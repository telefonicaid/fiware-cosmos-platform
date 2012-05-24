package es.tid.cosmos.tests.tasks;

/**
 *
 * @author ximo
 */
public class TestException extends RuntimeException {
    public TestException(String message) {
        super(message);
    }

    public TestException(String message, Throwable cause) {
        super(message, cause);
    }
}
