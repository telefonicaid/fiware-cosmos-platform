package es.tid.cosmos.base.mapreduce;

import org.apache.hadoop.fs.Path;
import static org.junit.Assert.assertEquals;
import org.junit.Test;

/**
 *
 * @author ximo
 */
public class CosmosJobTest {
    private static final Path NON_EXISITING_PATH = new Path("badproto://i/dont/exist");

    private static abstract class Generic<T, U> {
    }

    private static final class GenericImpl1 extends Generic<Integer, Integer> {
    }

    private static final class GenericImpl2 extends Generic<Integer, Generic<Integer, Integer>> {
    }

    @Test
    public void testGetGenericParameters1() throws Exception {
        GenericImpl1 l = new GenericImpl1();
        Class[] p = CosmosJob.getGenericParameters(l.getClass());
        assertEquals(p.length, 2);
        assertEquals(p[0], Integer.class);
        assertEquals(p[1], Integer.class);
    }

    @Test
    public void testGetGenericParameters2() throws Exception {
        GenericImpl2 l = new GenericImpl2();
        Class[] p = CosmosJob.getGenericParameters(l.getClass());
        assertEquals(p.length, 2);
        assertEquals(p[0], Integer.class);
        assertEquals(p[1], Generic.class);
    }
}
