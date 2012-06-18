package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;
import org.junit.Test;

import es.tid.cosmos.base.mapreduce.CosmosJob;
import es.tid.cosmos.base.mapreduce.CosmosWorkflow;
import es.tid.cosmos.base.mapreduce.WorkflowList;
import es.tid.cosmos.mobility.util.ExportPoiToTextByTwoIntReducer;

/**
 *
 * @author ximo
 */
public class DetectSecondHomesRunnerTest {
    @Test
    public void testRunner()
            throws IOException, InterruptedException, ClassNotFoundException {
        Configuration conf = new Configuration();
        conf.setBoolean("test", true);
        final Path dummyPath = new Path("/dummyPath");
        CosmosWorkflow wfrun = DetectSecondHomesRunner.run(
                dummyPath, dummyPath, dummyPath, dummyPath, dummyPath, dummyPath,
                true, conf);
        assertEquals(wfrun.getClass(), WorkflowList.class);
        WorkflowList list = (WorkflowList) wfrun;
        boolean exportToText = false;
        for (CosmosWorkflow wf : list) {
            assertEquals(wf.getClass(), CosmosJob.class);
            CosmosJob job = (CosmosJob) wf;
            assertTrue(job.getConfiguration().getBoolean("test", false));
            if (job.getReducerClass() == ExportPoiToTextByTwoIntReducer.class) {
                exportToText = true;
            }
        }
        
        assertTrue(exportToText);
    }
    
    @Test
    public void testRunnerNoDebug()
            throws IOException, InterruptedException, ClassNotFoundException {
        Configuration conf = new Configuration();
        conf.setBoolean("test", true);
        final Path dummyPath = new Path("/dummyPath");
        CosmosWorkflow wfrun = DetectSecondHomesRunner.run(
                dummyPath, dummyPath, dummyPath, dummyPath, dummyPath, dummyPath,
                false, conf);
        assertEquals(wfrun.getClass(), WorkflowList.class);
        WorkflowList list = (WorkflowList) wfrun;
        boolean exportToText = false;
        for (CosmosWorkflow wf : list) {
            assertEquals(wf.getClass(), CosmosJob.class);
            CosmosJob job = (CosmosJob) wf;
            assertTrue(job.getConfiguration().getBoolean("test", false));
            if (job.getReducerClass() == ExportPoiToTextByTwoIntReducer.class) {
                exportToText = true;
            }
        }
        
        assertFalse(exportToText);
    }
}
