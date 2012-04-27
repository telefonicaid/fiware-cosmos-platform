package es.tid.cosmos.base.mapreduce;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
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

    private static class FakeJob extends CosmosJob {
        private boolean waitForCompletionCalled;
        private Long submitCalledTime;
        private final boolean waitResult;

        public FakeJob(Configuration conf, String jobName, boolean waitResult)
                throws IOException {
            super(conf, jobName);
            this.waitResult = waitResult;
            this.waitForCompletionCalled = false;
            this.submitCalledTime = null;
        }

        @Override
        protected boolean callSuperWaitForCompletion(boolean verbose) {
            this.waitForCompletionCalled = true;
            return this.waitResult;
        }

        @Override
        protected void callSuperSubmit() throws InterruptedException {
            this.submitCalledTime = System.currentTimeMillis();
            Thread.sleep(10);
        }

        public Long getSubmitCallTime() {
            return this.submitCalledTime;
        }

        public boolean getWaitForCompletionCalled() {
            return this.waitForCompletionCalled;
        }
    }

    @Test
    public void testGetSet() throws Exception {
        FakeJob job = new FakeJob(new Configuration(), "Test", false);
        assertEquals(job.getDeleteOutputOnExit(), false);
        job.setDeleteOutputOnExit(true);
        assertEquals(job.getDeleteOutputOnExit(), true);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testDeleteOnExit() throws Exception {
        FakeJob job = new FakeJob(new Configuration(), "Test", true);
        job.setDeleteOutputOnExit(true);
        FileOutputFormat.setOutputPath(job,
                                       NON_EXISITING_PATH);
        job.waitForCompletion(true);
    }

    @Test(expected = JobExecutionException.class)
    public void testWaitForCompletion1() throws Exception {
        FakeJob job = new FakeJob(new Configuration(), "Test", false);
        job.waitForCompletion(true);
    }

    @Test(expected = JobExecutionException.class)
    public void testWaitForCompletion2() throws Exception {
        FakeJob job = new FakeJob(new Configuration(), "Test", false);
        job.submit();
        job.waitForCompletion(true);
    }

    @Test
    public void testWaitForCompletion3() throws Exception {
        FakeJob job = new FakeJob(new Configuration(), "Test", true);
        job.waitForCompletion(true);
        assertTrue(job.getWaitForCompletionCalled());
    }

    @Test
    public void testWaitForCompletion4() throws Exception {
        FakeJob job = new FakeJob(new Configuration(), "Test", true);
        job.submit();
        assertNotNull(job.getSubmitCallTime());
        job.waitForCompletion(true);
        assertTrue(job.getWaitForCompletionCalled());
    }

    @Test(expected = IllegalStateException.class)
    public void testWaitForCompletion5() throws Exception {
        FakeJob job = new FakeJob(new Configuration(), "Test", true);
        job.waitForCompletion(true);
        assertTrue(job.getWaitForCompletionCalled());
        job.addDependentJob(null);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testWaitForCompletion6() throws Exception {
        FakeJob job = new FakeJob(new Configuration(), "Test", true);
        job.addDependentJob(job);
    }

    @Test(expected = JobExecutionException.class)
    public void testWaitForCompletion7() throws Exception {
        FakeJob job = new FakeJob(new Configuration(), "Test", true);
        FakeJob job2 = new FakeJob(new Configuration(), "Test", false);
        job.addDependentJob(job2);
        job.waitForCompletion(true);
    }

    @Test
    public void testWaitForCompletion8() throws Exception {
        FakeJob job = new FakeJob(new Configuration(), "Test", true);
        FakeJob job2 = new FakeJob(new Configuration(), "Test", true);
        FakeJob job3 = new FakeJob(new Configuration(), "Test", true);
        FakeJob job4 = new FakeJob(new Configuration(), "Test", true);
        FakeJob job5 = new FakeJob(new Configuration(), "Test", true);
        job.addDependentJob(job2);
        job.addDependentJob(job3);
        job2.addDependentJob(job3);
        job2.addDependentJob(job4);
        job3.addDependentJob(job5);
        job.waitForCompletion(true);

        assertNotNull(job2.getSubmitCallTime());
        assertNotNull(job3.getSubmitCallTime());
        assertNotNull(job4.getSubmitCallTime());
        assertNotNull(job5.getSubmitCallTime());
        assertTrue(job2.getSubmitCallTime() > job3.getSubmitCallTime());
        assertTrue(job2.getSubmitCallTime() > job4.getSubmitCallTime());
        assertTrue(job3.getSubmitCallTime() > job5.getSubmitCallTime());

        assertTrue(job5.getWaitForCompletionCalled());
        assertTrue(job4.getWaitForCompletionCalled());
        assertTrue(job3.getWaitForCompletionCalled());
        assertTrue(job2.getWaitForCompletionCalled());
        assertTrue(job.getWaitForCompletionCalled());
    }

    @Test
    public void testWaitForCompletion9() throws Exception {
        FakeJob job = new FakeJob(new Configuration(), "Test", true);
        FakeJob job2 = new FakeJob(new Configuration(), "Test", true);
        FakeJob job3 = new FakeJob(new Configuration(), "Test", true);
        FakeJob job4 = new FakeJob(new Configuration(), "Test", true);
        FakeJob job5 = new FakeJob(new Configuration(), "Test", true);
        job.addDependentJob(job2);
        job2.addDependentJob(job3);
        job3.addDependentJob(job4);
        job4.addDependentJob(job5);
        job.waitForCompletion(true);

        assertNotNull(job2.getSubmitCallTime());
        assertNotNull(job3.getSubmitCallTime());
        assertNotNull(job4.getSubmitCallTime());
        assertNotNull(job5.getSubmitCallTime());
        assertTrue(job2.getSubmitCallTime() > job3.getSubmitCallTime());
        assertTrue(job3.getSubmitCallTime() > job4.getSubmitCallTime());
        assertTrue(job4.getSubmitCallTime() > job5.getSubmitCallTime());

        assertTrue(job5.getWaitForCompletionCalled());
        assertTrue(job4.getWaitForCompletionCalled());
        assertTrue(job3.getWaitForCompletionCalled());
        assertTrue(job2.getWaitForCompletionCalled());
        assertTrue(job.getWaitForCompletionCalled());
    }

    @Test(expected = IllegalStateException.class)
    public void testSubmit() throws Exception {
        FakeJob job = new FakeJob(new Configuration(), "Test", true);
        FakeJob job2 = new FakeJob(new Configuration(), "Test", false);
        job.addDependentJob(job2);
        job.submit();
    }
}
