package es.tid.cosmos.base.util;

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
        assertNotNull(Logger.get(LoggerTest.class));
        assertTrue(Logger.get(LoggerTest.class) instanceof
                org.apache.log4j.Logger);
    }
}
