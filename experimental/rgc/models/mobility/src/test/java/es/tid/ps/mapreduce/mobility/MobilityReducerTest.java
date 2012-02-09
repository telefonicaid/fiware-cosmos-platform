package es.tid.ps.mapreduce.mobility;

import static org.apache.hadoop.mrunit.testutil.ExtendedAssert.assertListEquals;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.NoSuchElementException;

import junit.framework.TestCase;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Counters;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.BlockJUnit4ClassRunner;

import es.tid.ps.mapreduce.mobility.data.CompositeKey;
import es.tid.ps.mapreduce.mobility.data.FileType;
import es.tid.ps.mapreduce.mobility.data.MobilityCounter;

/**
 * Test cases for the MobilityReducer.
 */
@RunWith(BlockJUnit4ClassRunner.class)
public class MobilityReducerTest extends  TestCase{

    private MobilityReducer reducer;
    private ReduceDriver<CompositeKey, Text, Text, LongWritable> driver;

    @Before
    public void setUp() {
        reducer = new MobilityReducer();
        driver = new ReduceDriver<CompositeKey, Text, Text, LongWritable>(
                reducer);
        driver.getConfiguration().set("mapred.cache.localArchives",
                "src/test/resources/cache_cells_data.txt");
    }

    @Test
    public void testOneWorkHour() {
        List<Pair<Text, LongWritable>> out = null;
        String actualUser = "33F43075348D57";

        reducer.updateUserInProcess(actualUser);
        driver.getConfiguration().set("models.mobility.work.cron",
                "* * 8-15 ? * 1-4");
        driver.getConfiguration().set("models.mobility.home.cron",
                "* * 16-23 ? * 1-7");

        driver.getConfiguration().set("mapred.cache.files",
                "src/test/resources/cache_cells_data.txt");
        try {
            List<Text> values = new ArrayList<Text>();
            values.add(new Text(
                    "0442941212677|33F43075348D57|0442941212677|33F43075348D57|2|01/06/2010|12:26:55|131|RMNACIO"));
            values.add(new Text(
                    "0442941212677|33F43075348D57|0442941212677|33F43075348D57|2|01/06/2010|12:26:55|131|RMNACIO"));
            values.add(new Text(
                    "0442941212676|33F43075348D57|0442941212676|33F43075348D57|2|01/06/2010|12:26:55|131|RMNACIO"));

            out = driver.withInputKey(new CompositeKey(actualUser, FileType.FILE_CDRS.getValue()))
                    .withInputValues(values).run();
        } catch (IOException ioe) {
            fail();
        }

        List<Pair<Text, LongWritable>> expected = new ArrayList<Pair<Text, LongWritable>>();
        expected.add(new Pair<Text, LongWritable>(new Text(actualUser
                + " work 0442941212677"), new LongWritable(4L)));
        expected.add(new Pair<Text, LongWritable>(new Text(actualUser
                + " work 13418"), new LongWritable(6L)));
        expected.add(new Pair<Text, LongWritable>(new Text(actualUser
                + " work 1000"), new LongWritable(6L)));
        expected.add(new Pair<Text, LongWritable>(new Text(actualUser
                + " work 1"), new LongWritable(6L)));

        assertListEquals(expected, out);
    }

    @Test
    public void testOneHomeHour() {
        List<Pair<Text, LongWritable>> out = null;
        String actualUser = "33F43075348D57";

        reducer.updateUserInProcess(actualUser);
        driver.getConfiguration().set("models.mobility.work.cron",
                "* * 8-15 ? * 1-4");
        driver.getConfiguration().set("models.mobility.home.cron",
                "* * 16-23 ? * 1-7");

        driver.getConfiguration().set("mapred.cache.files",
                "src/test/resources/cache_cells_data.txt");
        try {
            List<Text> values = new ArrayList<Text>();
            values.add(new Text(
                    "0442941212677|33F43075348D57|0442941212677|33F43075348D57|2|01/06/2010|18:26:55|131|RMNACIO"));

            out = driver.withInputKey(new CompositeKey(actualUser, FileType.FILE_CDRS.getValue()))
                    .withInputValues(values).run();
        } catch (IOException ioe) {
            fail();
        }

        List<Pair<Text, LongWritable>> expected = new ArrayList<Pair<Text, LongWritable>>();
        expected.add(new Pair<Text, LongWritable>(new Text(actualUser
                + " home 0442941212677"), new LongWritable(2L)));
        expected.add(new Pair<Text, LongWritable>(new Text(actualUser
                + " home 13418"), new LongWritable(2L)));
        expected.add(new Pair<Text, LongWritable>(new Text(actualUser
                + " home 1000"), new LongWritable(2L)));
        expected.add(new Pair<Text, LongWritable>(new Text(actualUser
                + " home 1"), new LongWritable(2L)));

        assertListEquals(expected, out);
    }

    @Test
    public void testNoLoadUser() {
        String loadUser = "NoLoad";
        String actualUser = "33F43075348D57";

        reducer.updateUserInProcess(loadUser);
        driver.getConfiguration().set("models.mobility.work.cron",
                "* * 8-15 ? * 1-4");
        driver.getConfiguration().set("models.mobility.home.cron",
                "* * 16-23 ? * 1-7");

        driver.getConfiguration().set("mapred.cache.files",
                "src/test/resources/cache_cells_data.txt");
        try {
            List<Text> values = new ArrayList<Text>();
            values.add(new Text(
                    "0442941212677|33F43075348D57|0442941212677|33F43075348D57|2|01/06/2010|18:26:55|131|RMNACIO"));

            driver.withInputKey(
                    new CompositeKey(actualUser, FileType.FILE_CDRS.getValue()))
                    .withInputValues(values).run();
        } catch (IOException ioe) {
            fail();
        }

        Counters expected = driver.getCounters();
        assertEquals(1L,
                expected.findCounter(MobilityCounter.USER_NOT_IN_PLATFORM)
                        .getValue());
    }
    
    @Test
    public void testErrorParserUser() {
        String loadUser = "33F43075348D57";
        String actualUser = "33F43075348D57";

        reducer.updateUserInProcess(loadUser);
        driver.getConfiguration().set("models.mobility.work.cron",
                "* * 8-15 ? * 1-4");
        driver.getConfiguration().set("models.mobility.home.cron",
                "* * 16-23 ? * 1-7");

        driver.getConfiguration().set("mapred.cache.files",
                "src/test/resources/cache_cells_data.txt");
        try {
            List<Text> values = new ArrayList<Text>();
            values.add(new Text(
                    "0442941212677"));

            driver.withInputKey(
                    new CompositeKey(actualUser, FileType.FILE_CDRS.getValue()))
                    .withInputValues(values).run();
        } catch (IOException ioe) {
            fail();
        }

        Counters expected = driver.getCounters();
        assertEquals(1L,
                expected.findCounter(MobilityCounter.LINE_PARSER_CDRS_ERROR)
                        .getValue());
    }

    @Test
    public void testUserFile() {
        String actualUser = "33F43075348D57";

        reducer.updateUserInProcess(actualUser);
        try {
            List<Text> values = new ArrayList<Text>();
            values.add(new Text(""));

            driver.withInputKey(
                    new CompositeKey(actualUser, FileType.FILE_USERS
                            .getValue())).withInputValues(values).run();
        } catch (IOException ioe) {
            fail();
        }
        assertEquals(reducer.actualUser, actualUser);
    }    
    
    @Test (expected=NoSuchElementException.class)
    public void testParseCellsFile() {
        String actualUser = "33F43075348D57";

        driver.getConfiguration().set("mapred.cache.files",
                "src/test/resources/cache_cells_data_bad.txt");        
        try {
            List<Text> values = new ArrayList<Text>();
            values.add(new Text(""));

            driver.withInputKey(
                    new CompositeKey(actualUser, FileType.FILE_USERS
                            .getValue())).withInputValues(values).run();
        } catch (IOException ioe) {
            fail();
        }
    }   
    
    @Test (expected=RuntimeException.class)
    public void testParseCron() {
        String actualUser = "33F43075348D57";

        driver.getConfiguration().set("models.mobility.work.cron",
                "erere");      
        try {
            List<Text> values = new ArrayList<Text>();
            values.add(new Text(""));

            driver.withInputKey(
                    new CompositeKey(actualUser, FileType.FILE_USERS
                            .getValue())).withInputValues(values).run();
        } catch (IOException ioe) {
            fail();
        }
    }
}
