package es.tid.cosmos.base.util;

import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

/**
 *
 * @author dmicol
 */
public class LoggerTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(Logger.class);
    }

    @Test
    public void testGet() {
        assertNotNull(Logger.get(LoggerTest.class));
        assertTrue(Logger.get(LoggerTest.class) instanceof
                org.apache.log4j.Logger);
    }
}
