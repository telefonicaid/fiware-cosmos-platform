package es.tid.bdp.kpicalculation.data;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author dmicol
 */
public class JobDetailsTest {
    private JobDetails instance;
    
    @Before
    public void setUp() {
        this.instance = new JobDetails("a", "b", "c");
    }
    
    @Test
    public void testGettersAndSetters() {
        assertEquals("a", this.instance.getName());
        assertEquals("b", this.instance.getFields());
        assertEquals("c", this.instance.getGroup());
        
        this.instance.setName("d");
        this.instance.setFields("e");
        this.instance.setGroup("f");
        assertEquals("d", this.instance.getName());
        assertEquals("e", this.instance.getFields());
        assertEquals("f", this.instance.getGroup());
    }
}
