package es.tid.cosmos.base.mapreduce;

import java.io.IOException;
import java.security.InvalidParameterException;
import java.util.LinkedList;
import java.util.List;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import static org.junit.Assert.assertEquals;
import org.junit.Test;

/**
 *
 * @author ximo
 */
public class FilePipelineTest {
    private static final Path NON_EXISITING_PATH = new Path("/tmp/i/dont/exist");

    private static class FakeJob extends CosmosJob {
        public FakeJob(String jobName)
                throws IOException {
            super(new Configuration(), jobName);
        }
    }

    @Test
    public void testGetJobs() throws IOException {
        FakeJob job = new FakeJob("test");
        FilePipeline pipe = new FilePipeline(job);
        List<CosmosJob> jobs = pipe.getJobs();
        assertEquals(jobs.size(), 1);
        assertEquals(jobs.get(0), job);

        FakeJob job2 = new FakeJob("test2");
        pipe = new FilePipeline(job, NON_EXISITING_PATH, job2);
        jobs = pipe.getJobs();
        assertEquals(jobs.size(), 2);
        assertEquals(jobs.get(0), job);
        assertEquals(jobs.get(1), job2);
    }

    @Test(expected = InvalidParameterException.class)
    public void testBadParams1() throws IOException {
        FilePipeline pipe = new FilePipeline(new FakeJob("test"), 0);
    }

    @Test(expected = InvalidParameterException.class)
    public void testBadParams2() throws IOException {
        FilePipeline pipe = new FilePipeline(new FakeJob("test"),
                                             NON_EXISITING_PATH);
    }

    @Test(expected = InvalidParameterException.class)
    public void testBadParams3() throws IOException {
        FilePipeline pipe = new FilePipeline(new FakeJob("test"),
                                             NON_EXISITING_PATH,
                                             0);
    }

    @Test(expected = InvalidParameterException.class)
    public void testBadParams4() throws IOException {
        FilePipeline pipe = new FilePipeline(new FakeJob("test"),
                                             NON_EXISITING_PATH,
                                             new Job());
    }

    @Test(expected = IOException.class)
    public void testWaitForCompletion() throws Exception {
        CosmosJob job = new FakeJob("test");
        FileInputFormat.setInputPaths(job, NON_EXISITING_PATH);
        FilePipeline pipe = new FilePipeline(job);
        pipe.waitForCompletion(true);
    }

    @Test(expected = InvalidParameterException.class)
    public void testMerge1() throws Exception {
        LinkedList<FakeJob> jobs = new LinkedList<FakeJob>();
        FakeJob job = new FakeJob("test");
        jobs.add(job);
        FilePipeline pipe1 = new FilePipeline(job);
        job = new FakeJob("test2");
        jobs.add(job);
        FilePipeline pipe2 = new FilePipeline(job);
        job = new FakeJob("test3");
        jobs.add(job);
        FilePipeline merge = FilePipeline.merge(new Runnable[]{pipe1, pipe2},
                                                job);
    }

    @Test(expected = IOException.class)
    public void testMerge2() throws Exception {
        LinkedList<FakeJob> jobs = new LinkedList<FakeJob>();
        FakeJob job = new FakeJob("test");
        jobs.add(job);
        FileOutputFormat.setOutputPath(job, NON_EXISITING_PATH);
        FilePipeline pipe1 = new FilePipeline(job);
        job = new FakeJob("test2");
        jobs.add(job);
        FileOutputFormat.setOutputPath(job, NON_EXISITING_PATH);
        FilePipeline pipe2 = new FilePipeline(job);
        job = new FakeJob("test3");
        jobs.add(job);
        FileOutputFormat.setOutputPath(job, NON_EXISITING_PATH);
        FilePipeline merge = FilePipeline.merge(new Runnable[]{pipe1, pipe2},
                                                job);
        merge.waitForCompletion(true);
    }
}
