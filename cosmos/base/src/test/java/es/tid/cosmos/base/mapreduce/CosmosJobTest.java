package es.tid.cosmos.base.mapreduce;

import java.io.IOException;

import com.mongodb.hadoop.io.BSONWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import org.junit.Test;

/**
 *
 * @author ximo
 */
public class CosmosJobTest {
    private static final Path NON_EXISITING_PATH = new Path(
            "badproto://i/dont/exist");

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
        private volatile Long submitCalledTime;
        private final boolean waitResult;

        public FakeJob(Configuration conf, String jobName, boolean waitResult)
                throws IOException {
            super(conf, jobName);
            this.waitResult = waitResult;
            this.waitForCompletionCalled = false;
            this.submitCalledTime = null;
        }

        @Override
        protected boolean callSuperWaitForCompletion(boolean verbose)
                throws InterruptedException {
            this.internalSubmit().join();
            this.waitForCompletionCalled = true;
            return this.waitResult;
        }

        private Thread internalSubmit() throws InterruptedException {
            if (this.submitCalledTime != null) {
                Thread dummy = new Thread();
                dummy.start();
                return dummy;
            }
            // Sleeping a bit to make sure currentTimeMillis changes between calls
            Thread.sleep(100);
            this.submitCalledTime = System.currentTimeMillis();
            Thread t = new Thread() {
                private static final long MAX_TIME = 100;
                private static final long MIN_TIME = 100;

                @Override
                public void run() {
                    try {
                        Thread.sleep(MIN_TIME
                                + (long)Math.random() * (MAX_TIME - MIN_TIME));
                    } catch (InterruptedException ex) {
                        // Ignore
                    }
                }
            };
            t.start();
            return t;
        }

        @Override
        protected void callSuperSubmit() throws InterruptedException {
            this.internalSubmit();
        }

        public Long getSubmitCallTime() throws InterruptedException {
            // Busy wait for submit to get called. Max wait is ~1.5 mins
            for (int i = 0;
                    i < 1000 && this.submitCalledTime == null;
                    i++, Thread.sleep(100));
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
        FakeJob job2 = new FakeJob(new Configuration(), "Test2", false);
        job.addDependentJob(job2);
        job.waitForCompletion(true);
    }

    @Test
    public void testWaitForCompletion8() throws Exception {
        FakeJob job = new FakeJob(new Configuration(), "Test", true);
        FakeJob job2 = new FakeJob(new Configuration(), "Test2", true);
        FakeJob job3 = new FakeJob(new Configuration(), "Test3", true);
        FakeJob job4 = new FakeJob(new Configuration(), "Test4", true);
        FakeJob job5 = new FakeJob(new Configuration(), "Test5", true);
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
        FakeJob job2 = new FakeJob(new Configuration(), "Test2", true);
        FakeJob job3 = new FakeJob(new Configuration(), "Test3", true);
        FakeJob job4 = new FakeJob(new Configuration(), "Test4", true);
        FakeJob job5 = new FakeJob(new Configuration(), "Test5", true);
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

    @Test
    public void testWaitForCompletion10() throws Exception {
        FakeJob main = new FakeJob(new Configuration(), "Main", true);
        FakeJob agg = new FakeJob(new Configuration(), "Agg", true);
        FakeJob agg2 = new FakeJob(new Configuration(), "Agg2", true);
        FakeJob agg3 = new FakeJob(new Configuration(), "Agg3", true);
        FakeJob exp = new FakeJob(new Configuration(), "Exp", true);
        FakeJob exp2 = new FakeJob(new Configuration(), "Exp2", true);
        FakeJob exp3 = new FakeJob(new Configuration(), "Exp3", true);
        agg.addDependentJob(main);
        agg2.addDependentJob(main);
        agg3.addDependentJob(main);
        exp.addDependentJob(agg);
        exp2.addDependentJob(agg2);
        exp3.addDependentJob(agg3);

        JobList j = new JobList();
        j.add(exp);
        j.add(exp2);
        j.add(exp3);

        j.waitForCompletion(true);

        assertTrue(main.getSubmitCallTime() < agg.getSubmitCallTime());
        assertTrue(main.getSubmitCallTime() < agg2.getSubmitCallTime());
        assertTrue(main.getSubmitCallTime() < agg3.getSubmitCallTime());
        assertTrue(agg.getSubmitCallTime() < exp.getSubmitCallTime());
        assertTrue(agg2.getSubmitCallTime() < exp2.getSubmitCallTime());
        assertTrue(agg3.getSubmitCallTime() < exp3.getSubmitCallTime());

        assertTrue(main.getWaitForCompletionCalled());
        assertTrue(agg.getWaitForCompletionCalled());
        assertTrue(agg2.getWaitForCompletionCalled());
        assertTrue(agg3.getWaitForCompletionCalled());
        assertTrue(exp.getWaitForCompletionCalled());
        assertTrue(exp2.getWaitForCompletionCalled());
        assertTrue(exp3.getWaitForCompletionCalled());
    }

    private static class DummyMapper
            extends Mapper<LongWritable, Text, NullWritable, BSONWritable> {
    }

    private static class DummyReducer
            extends Reducer<NullWritable, BSONWritable, Text, Text> {
    }

    private static class DummyReducer2
            extends Reducer<LongWritable, Text, Text, Text> {
    }

    @Test
    public void testCreate1() throws Exception {
        final String jobName = "MapReduceJobName";
        CosmosJob job = CosmosJob.createMapReduceJob(
                new Configuration(), jobName,
                TextInputFormat.class, DummyMapper.class,
                DummyReducer.class, TextOutputFormat.class);
        assertEquals(job.getJobName(), jobName);
        assertEquals(job.getInputFormatClass(), TextInputFormat.class);
        assertEquals(job.getMapOutputKeyClass(), NullWritable.class);
        assertEquals(job.getMapOutputValueClass(), BSONWritable.class);
        assertEquals(job.getOutputKeyClass(), Text.class);
        assertEquals(job.getOutputValueClass(), Text.class);
        assertEquals(job.getReducerClass(), DummyReducer.class);
        assertEquals(job.getMapperClass(), DummyMapper.class);
    }

    @Test
    public void testCreate2() throws Exception {
        final String jobName = "MapReduceJobName";
        final int reduceTasks = 8;
        CosmosJob job = CosmosJob.createMapReduceJob(
                new Configuration(), jobName,
                TextInputFormat.class, DummyMapper.class,
                DummyReducer.class, reduceTasks, TextOutputFormat.class);
        assertEquals(job.getJobName(), jobName);
        assertEquals(job.getInputFormatClass(), TextInputFormat.class);
        assertEquals(job.getMapOutputKeyClass(), NullWritable.class);
        assertEquals(job.getMapOutputValueClass(), BSONWritable.class);
        assertEquals(job.getOutputKeyClass(), Text.class);
        assertEquals(job.getOutputValueClass(), Text.class);
        assertEquals(job.getReducerClass(), DummyReducer.class);
        assertEquals(job.getMapperClass(), DummyMapper.class);
        assertEquals(job.getNumReduceTasks(), reduceTasks);
    }

    @Test
    public void testCreate3() throws Exception {
        final String jobName = "MapperJobName";
        CosmosJob job = CosmosJob.createMapJob(
                new Configuration(), jobName, TextInputFormat.class,
                DummyMapper.class, TextOutputFormat.class);
        assertEquals(job.getJobName(), jobName);
        assertEquals(job.getInputFormatClass(), TextInputFormat.class);
        assertEquals(job.getMapOutputKeyClass(), NullWritable.class);
        assertEquals(job.getMapOutputValueClass(), BSONWritable.class);
        assertEquals(job.getOutputKeyClass(), NullWritable.class);
        assertEquals(job.getOutputValueClass(), BSONWritable.class);
        assertEquals(job.getReducerClass(), Reducer.class);
        assertEquals(job.getMapperClass(), DummyMapper.class);
    }

    @Test
    public void testCreate4() throws Exception {
        final String jobName = "ReduceJobName";
        CosmosJob job = CosmosJob.createReduceJob(
                new Configuration(), jobName, TextInputFormat.class,
                DummyReducer2.class, TextOutputFormat.class);
        assertEquals(job.getJobName(), jobName);
        assertEquals(job.getInputFormatClass(), TextInputFormat.class);
        assertEquals(job.getMapOutputKeyClass(), LongWritable.class);
        assertEquals(job.getMapOutputValueClass(), Text.class);
        assertEquals(job.getOutputKeyClass(), Text.class);
        assertEquals(job.getOutputValueClass(), Text.class);
        assertEquals(job.getReducerClass(), DummyReducer2.class);
        assertEquals(job.getMapperClass(), Mapper.class);
    }

    @Test
    public void testCreate5() throws Exception {
        final String jobName = "ReduceJobName";
        final int reduceTasks = 4;
        CosmosJob job = CosmosJob.createReduceJob(
                new Configuration(), jobName, TextInputFormat.class,
                DummyReducer2.class, reduceTasks, TextOutputFormat.class);
        assertEquals(job.getJobName(), jobName);
        assertEquals(job.getInputFormatClass(), TextInputFormat.class);
        assertEquals(job.getMapOutputKeyClass(), LongWritable.class);
        assertEquals(job.getMapOutputValueClass(), Text.class);
        assertEquals(job.getOutputKeyClass(), Text.class);
        assertEquals(job.getOutputValueClass(), Text.class);
        assertEquals(job.getReducerClass(), DummyReducer2.class);
        assertEquals(job.getMapperClass(), Mapper.class);
        assertEquals(job.getNumReduceTasks(), reduceTasks);
    }

    @Test(expected=JobExecutionException.class)
    public void testSubmit() throws Exception {
        FakeJob job = new FakeJob(new Configuration(), "Test", true);
        FakeJob job2 = new FakeJob(new Configuration(), "Test2", false);
        job.addDependentJob(job2);
        job.submit();
        assertNotNull(job2.getSubmitCallTime());
        job.waitForCompletion(true);
    }
}
