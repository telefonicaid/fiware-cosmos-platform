package es.tid.cosmos.base.mapreduce;

import java.io.IOException;
import java.security.InvalidParameterException;
import java.util.EnumSet;
import java.util.LinkedList;
import java.util.List;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import org.junit.Test;

/**
 *
 * @author ximo
 */
public class FilePipelineTest {
    private static final Path NON_EXISITING_PATH = new Path("/tmp/i/dont/exist");

    private static class FakeJob extends CosmosJob {
        private boolean waitResult;
        private boolean called;

        public FakeJob(String jobName, boolean waitResult)
                throws IOException {
            super(new Configuration(), jobName);
            this.waitResult = waitResult;
            this.called = false;
        }

        @Override
        public boolean waitForCompletion(boolean dummy) {
            this.called = true;
            return this.waitResult;
        }

        public boolean called() {
            return this.called;
        }
    }

    @Test
    public void testGetJobs() throws IOException {
        FakeJob job = new FakeJob("test", false);
        FilePipeline pipe = new FilePipeline(job);
        List<CosmosJob> jobs = pipe.getJobs();
        assertEquals(jobs.size(), 1);
        assertEquals(jobs.get(0), job);

        FakeJob job2 = new FakeJob("test2", false);
        pipe = new FilePipeline(job, NON_EXISITING_PATH, job2);
        jobs = pipe.getJobs();
        assertEquals(jobs.size(), 2);
        assertEquals(jobs.get(0), job);
        assertEquals(jobs.get(1), job2);
    }

    @Test(expected = InvalidParameterException.class)
    public void testBadParams1() throws IOException {
        FilePipeline pipe = new FilePipeline(new FakeJob("test", true), 0);
    }

    @Test(expected = InvalidParameterException.class)
    public void testBadParams2() throws IOException {
        FilePipeline pipe = new FilePipeline(new FakeJob("test", true),
                                             NON_EXISITING_PATH);
    }

    @Test(expected = InvalidParameterException.class)
    public void testBadParams3() throws IOException {
        FilePipeline pipe = new FilePipeline(new FakeJob("test", true),
                                             NON_EXISITING_PATH,
                                             0);
    }

    @Test(expected = InvalidParameterException.class)
    public void testBadParams4() throws IOException {
        FilePipeline pipe = new FilePipeline(new FakeJob("test", true),
                                             NON_EXISITING_PATH,
                                             new Job());
    }

    @Test(expected = Exception.class)
    public void testWaitForCompletion1() throws Exception {
        FilePipeline pipe = new FilePipeline(new FakeJob("test", false));
        pipe.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
    }

    @Test(expected = Exception.class)
    public void testWaitForCompletion2() throws Exception {
        FilePipeline pipe = new FilePipeline(new FakeJob("test", true),
                                             NON_EXISITING_PATH,
                                             new FakeJob("test2", false));
        pipe.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
    }

    @Test
    public void testWaitForCompletion3() throws Exception {
        FilePipeline pipe = new FilePipeline(new FakeJob("test", true),
                                             NON_EXISITING_PATH,
                                             new FakeJob("test2", true));
        pipe.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
    }

    @Test
    public void testWaitForCompletion4() throws Exception {
        FilePipeline pipe = new FilePipeline(new FakeJob("test", true));
        pipe.waitForCompletion(
                EnumSet.of(CleanupOptions.DeleteIntermediateResults,
                           CleanupOptions.DeleteInput));
    }

    @Test(expected = Exception.class)
    public void testWaitForCompletion5() throws Exception {
        FilePipeline pipe = new FilePipeline(new FakeJob("test", true),
                                             NON_EXISITING_PATH,
                                             new FakeJob("test2", true));
        pipe.waitForCompletion(
                EnumSet.of(CleanupOptions.DeleteIntermediateResults));
    }

    @Test(expected = Exception.class)
    public void testWaitForCompletion6() throws Exception {
        FilePipeline pipe = new FilePipeline(new FakeJob("test", true),
                                             NON_EXISITING_PATH,
                                             new FakeJob("test2", true));
        pipe.waitForCompletion(
                EnumSet.of(CleanupOptions.DeleteOutput));
    }

    @Test(expected = Exception.class)
    public void testWaitForCompletion7() throws Exception {
        FilePipeline pipe = new FilePipeline(new FakeJob("test", true));
        pipe.waitForCompletion(
                EnumSet.of(CleanupOptions.DeleteOutput));
    }

    @Test(expected = InvalidParameterException.class)
    public void testMerge1() throws Exception {
        LinkedList<FakeJob> jobs = new LinkedList<FakeJob>();
        FakeJob job = new FakeJob("test", true);
        jobs.add(job);
        FilePipeline pipe1 = new FilePipeline(job);
        job = new FakeJob("test2", true);
        jobs.add(job);
        FilePipeline pipe2 = new FilePipeline(job);
        job = new FakeJob("test3", true);
        jobs.add(job);
        FilePipeline merge = FilePipeline.merge(new Runnable[]{pipe1, pipe2},
                                                job);
    }

    @Test
    public void testMerge2() throws Exception {
        LinkedList<FakeJob> jobs = new LinkedList<FakeJob>();
        FakeJob job = new FakeJob("test", true);
        jobs.add(job);
        FileOutputFormat.setOutputPath(job, NON_EXISITING_PATH);
        FilePipeline pipe1 = new FilePipeline(job);
        job = new FakeJob("test2", true);
        jobs.add(job);
        FileOutputFormat.setOutputPath(job, NON_EXISITING_PATH);
        FilePipeline pipe2 = new FilePipeline(job);
        job = new FakeJob("test3", true);
        jobs.add(job);
        FileOutputFormat.setOutputPath(job, NON_EXISITING_PATH);
        FilePipeline merge = FilePipeline.merge(new Runnable[]{pipe1, pipe2},
                                                job);
        merge.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        for (FakeJob currentJob : jobs) {
            assertTrue(currentJob.called());
        }
    }
}
