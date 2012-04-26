package es.tid.cosmos.base.mapreduce;

import java.io.IOException;
import java.util.List;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import static org.junit.Assert.assertEquals;
import org.junit.Test;

/**
 *
 * @author ximo
 */
public class CosmosJobTest {
    private static final Path NON_EXISITING_PATH = new Path("badproto://i/dont/exist");

    private static class FakeJob extends CosmosJob {
        public FakeJob(String jobName) throws IOException {
            super(new Configuration(), jobName);
        }
    }

    @Test
    public void testGetJobs() throws IOException {
        final String name = "test";
        FakeJob job = new FakeJob(name);
        assertEquals(job.getJobName(), name);
        List<CosmosJob> jobs = job.getJobs();
        assertEquals(jobs.size(), 1);
        assertEquals(jobs.get(0), job);
    }

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

    //@Test
    // TODO: Enable this test once dmicol pulls the fix for this in his
    // mobility model
    public void testGetGenericParameters2() throws Exception {
        GenericImpl2 l = new GenericImpl2();
        Class[] p = CosmosJob.getGenericParameters(l.getClass());
        assertEquals(p.length, 2);
        assertEquals(p[0], Integer.class);
        assertEquals(p[1], Generic.class);
    }
}
