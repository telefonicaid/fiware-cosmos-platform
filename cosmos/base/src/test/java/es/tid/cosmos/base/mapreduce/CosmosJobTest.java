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
}
