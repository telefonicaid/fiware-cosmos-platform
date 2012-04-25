package es.tid.cosmos.base.mapreduce;

import java.io.IOException;
import java.util.EnumSet;
import java.util.List;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import static org.junit.Assert.assertEquals;
import org.junit.Test;

/**
 *
 * @author ximo
 */
public class CosmosJobTest {
    private static final Path NON_EXISITING_PATH = new Path("badproto://i/dont/exist");

    private static class FakeJob extends CosmosJob {
        private boolean waitResult;

        public FakeJob(String jobName, boolean waitResult)
                throws IOException {
            super(new Configuration(), jobName);
            this.waitResult = waitResult;
        }

        @Override
        public boolean waitForCompletion(boolean dummy) {
            return this.waitResult;
        }
    }

    @Test
    public void testGetJobs() throws IOException {
        final String name = "test";
        FakeJob job = new FakeJob(name, false);
        assertEquals(job.getJobName(), name);
        List<CosmosJob> jobs = job.getJobs();
        assertEquals(jobs.size(), 1);
        assertEquals(jobs.get(0), job);
    }

    @Test
    public void testWaitForCompletion1() throws Exception {
        final String name = "test";
        FakeJob job = new FakeJob(name, true);
        job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
    }

    @Test(expected = Exception.class)
    public void testWaitForCompletion2() throws Exception {
        final String name = "test";
        FakeJob job = new FakeJob(name, false);
        job.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
    }

    @Test
    public void testWaitForCompletion3() throws Exception {
        final String name = "test";
        FakeJob job = new FakeJob(name, true);
        job.waitForCompletion(
                EnumSet.of(CleanupOptions.DeleteIntermediateResults));
    }

    @Test(expected = Exception.class)
    public void testWaitForCompletion4() throws Exception {
        final String name = "test";
        FakeJob job = new FakeJob(name, true);
        job.waitForCompletion(
                EnumSet.of(CleanupOptions.DeleteOutput));
    }

    @Test
    public void testWaitForCompletion5() throws Exception {
        final String name = "test";
        FakeJob job = new FakeJob(name, true);
        job.waitForCompletion(
                EnumSet.of(CleanupOptions.DeleteInput));
    }

    @Test(expected = Exception.class)
    public void testWaitForCompletion6() throws Exception {
        final String name = "test";
        FakeJob job = new FakeJob(name, true);
        FileInputFormat.setInputPaths(job, NON_EXISITING_PATH);
        job.waitForCompletion(
                EnumSet.of(CleanupOptions.DeleteInput));
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

    @Test
    public void testGetGenericParameters2() throws Exception {
        GenericImpl2 l = new GenericImpl2();
        Class[] p = CosmosJob.getGenericParameters(l.getClass());
        assertEquals(p.length, 2);
        assertEquals(p[0], Integer.class);
        assertEquals(p[1], Generic.class);
    }
}
