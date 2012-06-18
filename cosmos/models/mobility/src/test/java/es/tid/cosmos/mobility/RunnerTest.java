package es.tid.cosmos.mobility;

import java.lang.reflect.Method;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import org.junit.Test;

import es.tid.cosmos.base.mapreduce.CosmosJob;
import es.tid.cosmos.base.mapreduce.CosmosWorkflow;
import es.tid.cosmos.base.mapreduce.WorkflowList;
import es.tid.cosmos.mobility.activitydensity.ActivityDensityRunner;
import es.tid.cosmos.mobility.activitydensity.profile.ActivityDensityProfileRunner;
import es.tid.cosmos.mobility.aggregatedmatrix.group.AggregatedMatrixGroupRunner;
import es.tid.cosmos.mobility.aggregatedmatrix.simple.AggregatedMatrixSimpleRunner;
import es.tid.cosmos.mobility.labelling.bts.BtsLabellingRunner;
import es.tid.cosmos.mobility.labelling.client.ClientLabellingRunner;
import es.tid.cosmos.mobility.labelling.clientbts.ClientBtsLabellingRunner;
import es.tid.cosmos.mobility.labelling.join.LabelJoiningRunner;
import es.tid.cosmos.mobility.labelling.secondhomes.DetectSecondHomesRunner;
import es.tid.cosmos.mobility.mivs.MivsRunner;
import es.tid.cosmos.mobility.outpois.OutPoisRunner;
import es.tid.cosmos.mobility.parsing.ParsingRunner;
import es.tid.cosmos.mobility.pois.PoisRunner;
import es.tid.cosmos.mobility.populationdensity.PopulationDensityRunner;
import es.tid.cosmos.mobility.populationdensity.profile.PopulationDensityProfileRunner;
import es.tid.cosmos.mobility.preparing.PreparingRunner;
import es.tid.cosmos.mobility.util.ExportClusterToTextByTwoIntReducer;
import es.tid.cosmos.mobility.util.ExportClusterToTextReducer;
import es.tid.cosmos.mobility.util.ExportPoiToTextByTwoIntReducer;
import es.tid.cosmos.mobility.util.ExportPoiToTextReducer;

/**
 *
 * @author dmicol, ximo
 */
public class RunnerTest {
    private static final Class[] RUNNER_CLASSES = {
        ActivityDensityRunner.class,
        ActivityDensityProfileRunner.class,
        AggregatedMatrixGroupRunner.class,
        AggregatedMatrixSimpleRunner.class,
        BtsLabellingRunner.class,
        ClientLabellingRunner.class,
        ClientBtsLabellingRunner.class,
        LabelJoiningRunner.class,
        DetectSecondHomesRunner.class,
        MivsRunner.class,
        ParsingRunner.class,
        PoisRunner.class,
        PopulationDensityRunner.class,
        PopulationDensityProfileRunner.class,
        PreparingRunner.class,
    };
    private static final Class[] EXPORT_TO_TEXT_CLASSES = {
        null,
        null,
        null,
        null,
        ExportClusterToTextReducer.class,
        ExportClusterToTextReducer.class,
        ExportClusterToTextByTwoIntReducer.class,
        ExportPoiToTextReducer.class,
        ExportPoiToTextByTwoIntReducer.class,
        null,
        null,
        null,
        null,
        null,
        null,
    };
    
    private static void testRunner(Class runnerClazz, Class exportToTextClazz,
            boolean isDebug) throws Exception {
        Configuration conf = new Configuration();
        conf.setBoolean("test", true);
        final Path dummyPath = new Path("/dummyPath");
        final Method runnerMethod = runnerClazz.getMethods()[0];
        final int numberOfArguments = runnerMethod.getParameterTypes().length;
        Object[] methodData = new Object[numberOfArguments];
        for (int i = 0; i < numberOfArguments - 2; i++) {
            methodData[i] = dummyPath;
        }
        methodData[numberOfArguments - 2] = isDebug;
        methodData[numberOfArguments - 1] = conf;
        CosmosWorkflow wfrun = (CosmosWorkflow) runnerMethod.invoke(
                runnerClazz.newInstance(), methodData);
        assertEquals(wfrun.getClass(), WorkflowList.class);
        WorkflowList list = (WorkflowList) wfrun;
        boolean exportToText = false;
        for (CosmosWorkflow wf : list) {
            assertEquals(wf.getClass(), CosmosJob.class);
            CosmosJob job = (CosmosJob) wf;
            assertTrue(job.getConfiguration().getBoolean("test", false));
            if (job.getReducerClass() == exportToTextClazz) {
                exportToText = true;
            }
        }
        if (exportToTextClazz != null) {
            assertEquals(exportToText, isDebug);
        }
    }
    
    private static void testRunners(boolean isDebug) throws Exception {
        assertEquals(RUNNER_CLASSES.length, EXPORT_TO_TEXT_CLASSES.length);
        for (int i = 0; i < RUNNER_CLASSES.length; i++) {
            testRunner(RUNNER_CLASSES[i], EXPORT_TO_TEXT_CLASSES[i], isDebug);
        }
    }
    
    @Test
    public void testRunnersDebug() throws Exception {
        testRunners(true);
    }
    
    @Test
    public void testRunnersNoDebug() throws Exception {
        testRunners(false);
    }
}
