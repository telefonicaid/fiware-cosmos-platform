package es.tid.cosmos.mobility.util;

import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import org.junit.Test;

/**
 *
 * @author dmicol
 */
public class LoggerTest {
    @Test
    public void testGet() {
        assertNotNull(Logger.get());
        assertTrue(Logger.get() instanceof org.apache.log4j.Logger);
    }
}
