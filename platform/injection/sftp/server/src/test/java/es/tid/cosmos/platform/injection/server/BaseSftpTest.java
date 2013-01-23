package es.tid.cosmos.platform.injection.server;

import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.junit.Before;

/**
 * Suppress logging during SFTP tests execution. All warnings issued to
 * console are correctly caused by the tests, but they clutter the output
 * terminal.
 *
 * @author logc
 */
public class BaseSftpTest {

    private final Logger logger;

    public BaseSftpTest(Logger logger) {
        this.logger = logger;
    }

    @Before
    public void disableLogging() {
        this.logger.setLevel(Level.OFF);
    }
}
