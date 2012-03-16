package es.tid.mapreduce.mobility2;

import static org.apache.hadoop.mrunit.testutil.ExtendedAssert.assertListEquals;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;

import junit.framework.TestCase;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Counters;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.BlockJUnit4ClassRunner;

import es.tid.mapreduce.mobility2.data.Mobility2Counter;
import es.tid.mapreduce.mobility2.data.UserMobilityData;

@RunWith(BlockJUnit4ClassRunner.class)
public class Mobility2ReducerTest extends TestCase {

    private Mobility2Reducer reduce;
    private ReduceDriver<Text, UserMobilityData, Text, Text> driver;

    @Before
    public void setUp() {
        reduce = new Mobility2Reducer();
        driver = new ReduceDriver<Text, UserMobilityData, Text, Text>(reduce);
        driver.getConfiguration().set("mapred.cache.files",
                "src/test/resources/represent_vector.dat");
    }

    @Test
    public void oneBts() {

        List<Pair<Text, Text>> out = null;
        try {
            Calendar cal = Calendar.getInstance();
            cal.setTimeInMillis(1275382669000L);

            List<UserMobilityData> values = new ArrayList<UserMobilityData>();

            values.add(new UserMobilityData("13418", cal));
            values.add(new UserMobilityData("13418", cal));
            values.add(new UserMobilityData("13418", cal));
            values.add(new UserMobilityData("13418", cal));
            values.add(new UserMobilityData("13418", cal));
            values.add(new UserMobilityData("13418", cal));
            values.add(new UserMobilityData("13418", cal));
            values.add(new UserMobilityData("13418", cal));
            values.add(new UserMobilityData("13418", cal));
            values.add(new UserMobilityData("13418", cal));
            values.add(new UserMobilityData("13418", cal));
            values.add(new UserMobilityData("13418", cal));
            values.add(new UserMobilityData("13418", cal));
            values.add(new UserMobilityData("13418", cal));

            out = driver.withInputKey(new Text("user")).withInputValues(values)
                    .run();
        } catch (IOException ioe) {
            fail();
        }

        List<Pair<Text, Text>> expected = new ArrayList<Pair<Text, Text>>();
        expected.add(new Pair<Text, Text>(new Text("user test 13418"),
                new Text("-0.010526315789473672")));

        assertListEquals(expected, out);
    }

    @Test
    public void twoBts() {

        List<Pair<Text, Text>> out = null;
        try {
            Calendar cal = Calendar.getInstance();
            Calendar cal2 = Calendar.getInstance();

            cal2.setTimeInMillis(1275382669000L);
            cal.setTimeInMillis(1276382669000L);

            List<UserMobilityData> values = new ArrayList<UserMobilityData>();

            values.add(new UserMobilityData("13418", cal2));
            values.add(new UserMobilityData("13418", cal2));
            values.add(new UserMobilityData("13418", cal2));
            values.add(new UserMobilityData("13418", cal2));
            values.add(new UserMobilityData("13418", cal2));
            values.add(new UserMobilityData("13418", cal2));
            values.add(new UserMobilityData("13418", cal2));
            values.add(new UserMobilityData("13418", cal2));
            values.add(new UserMobilityData("13418", cal2));
            values.add(new UserMobilityData("13418", cal2));
            values.add(new UserMobilityData("13418", cal2));
            values.add(new UserMobilityData("13418", cal2));
            values.add(new UserMobilityData("13418", cal2));
            values.add(new UserMobilityData("13418", cal2));

            values.add(new UserMobilityData("134198", cal));
            values.add(new UserMobilityData("134198", cal));
            values.add(new UserMobilityData("134198", cal));
            values.add(new UserMobilityData("134198", cal));
            values.add(new UserMobilityData("134198", cal));
            values.add(new UserMobilityData("134198", cal));
            values.add(new UserMobilityData("134198", cal));
            values.add(new UserMobilityData("134198", cal));
            values.add(new UserMobilityData("134198", cal));
            values.add(new UserMobilityData("134198", cal));
            values.add(new UserMobilityData("134198", cal));
            values.add(new UserMobilityData("134198", cal));
            values.add(new UserMobilityData("134198", cal));
            values.add(new UserMobilityData("134198", cal));

            out = driver.withInputKey(new Text("user")).withInputValues(values)
                    .run();
        } catch (IOException ioe) {
            fail();
        }

        List<Pair<Text, Text>> expected = new ArrayList<Pair<Text, Text>>();
        expected.add(new Pair<Text, Text>(new Text("user test 13418"),
                new Text("-0.010526315789473672")));

        assertListEquals(expected, out);
    }

    @Test
    public void excedNumber() {
        try {
            Calendar cal = Calendar.getInstance();
            cal.setTimeInMillis(1276382669000L);

            List<UserMobilityData> values = new ArrayList<UserMobilityData>();
            for (int i = 0; i < 5002; ++i) {
                values.add(new UserMobilityData("13418", cal));
            }

            driver.withInputKey(new Text("user")).withInputValues(values).run();
        } catch (IOException ioe) {
            fail();
        }

        Counters expected = driver.getCounters();
        assertEquals(1L, expected
                .findCounter(Mobility2Counter.USER_CRDS_EXCEED).getValue());
    }
    
    @Test
    public void emptyBts() {

        List<Pair<Text, Text>> out = null;
        try {
            Calendar cal = Calendar.getInstance();
            cal.setTimeInMillis(1275382669000L);

            List<UserMobilityData> values = new ArrayList<UserMobilityData>();

            values.add(new UserMobilityData("", cal));
            values.add(new UserMobilityData("", cal));
            values.add(new UserMobilityData("", cal));
            values.add(new UserMobilityData("", cal));
            values.add(new UserMobilityData("", cal));
            values.add(new UserMobilityData("", cal));
            values.add(new UserMobilityData("", cal));
            values.add(new UserMobilityData("", cal));
            values.add(new UserMobilityData("", cal));
            values.add(new UserMobilityData("", cal));
            values.add(new UserMobilityData("", cal));
            values.add(new UserMobilityData("", cal));
            values.add(new UserMobilityData("", cal));
            values.add(new UserMobilityData("", cal));

            out = driver.withInputKey(new Text("user")).withInputValues(values)
                    .run();
        } catch (IOException ioe) {
            fail();
        }

        List<Pair<Text, Text>> expected = new ArrayList<Pair<Text, Text>>();
        assertListEquals(expected, out);
    }


}