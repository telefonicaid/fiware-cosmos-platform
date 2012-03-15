package es.tid.bdp.kpicalculation.data;

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.bdp.kpicalculation.config.JobDetails;

/**
 *
 * @author dmicol
 */
public class JobDetailsTest {
    private JobDetails instance;

    @Before
    public void setUp() {
        this.instance = new JobDetails("a", new String[] { "b" });
    }

    @Test
    public void testGettersAndSetters() {
        assertEquals("a", this.instance.getName());
        assertArrayEquals(new String[] { "b" }, this.instance.getFields());
        assertEquals(null, this.instance.getGroup());
    }
}
