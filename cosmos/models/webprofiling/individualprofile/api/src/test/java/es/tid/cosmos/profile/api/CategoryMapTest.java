package es.tid.cosmos.profile.api;

import static java.util.Arrays.asList;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author dmicol, sortega
 */
public class CategoryMapTest {
    private CategoryMap instance;

    @Before
    public void setUp() {
        this.instance = new CategoryMap();
        this.instance.put("B", 20L);
        this.instance.put("A", 30L);
        this.instance.put("C", 10L);
        this.instance.put("D",  5L);
    }

    @Test
    public void shouldOrderByCount() {
        assertEquals(asList("A", "B", "C"), this.instance.getTop(3));
    }

    @Test
    public void lessThanNecessaryTest() {
        assertEquals(asList("A", "B", "C", "D"), this.instance.getTop(5));
    }
    
    @Test
    public void shouldGetAll() {
        assertEquals(asList("A", "B", "C", "D"), this.instance.getAll());
    }
}
