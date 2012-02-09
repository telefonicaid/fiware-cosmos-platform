package es.tid.ps.mapreduce.sna;

import static org.apache.hadoop.mrunit.testutil.ExtendedAssert.assertListEquals;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import junit.framework.TestCase;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.BlockJUnit4ClassRunner;

/**
 * Test cases for the Cliques Calculator reducer.
 * 
 * @author rgc@tid.es
 */
@RunWith(BlockJUnit4ClassRunner.class)
public class CliquesCalculatorReducerTest extends TestCase {

    private Reducer<Text, NodeCombination, Text, Text> reducer;
    private ReduceDriver<Text, NodeCombination, Text, Text> driver;

    @Before
    public void setUp() {
        reducer = new CliquesCalculatorReducer();
        driver = new ReduceDriver<Text, NodeCombination, Text, Text>(reducer);
    }

    @Test
    public void testCliquesFromNode() {
        List<Pair<Text, Text>> out = null;
        Text key = new Text("5");
        List<NodeCombination> values = new ArrayList<NodeCombination>();
        values.add(new NodeCombination("4", new String[] { "7" }));
        values.add(new NodeCombination("6", new String[] { "7", "8" }));
        values.add(new NodeCombination("7", new String[] { "4", "6", "8" }));
        values.add(new NodeCombination("8", new String[] { "6", "7" }));

        try {
            out = driver.withInputKey(key).withInputValues(values).run();
        } catch (IOException ioe) {
            fail();
        }

        List<Pair<Text, Text>> expected = new ArrayList<Pair<Text, Text>>();
        expected.add(new Pair<Text, Text>(new Text("5,4,7"), new Text("")));
        expected.add(new Pair<Text, Text>(new Text("5,6,7,8"), new Text("")));

        assertListEquals(expected, out);
    }
    
    @Test
    public void testCliquesElementInTwo() {
        List<Pair<Text, Text>> out = null;
        Text key = new Text("7");
        List<NodeCombination> values = new ArrayList<NodeCombination>();
        values.add(new NodeCombination("4", new String[] { "5" }));
        values.add(new NodeCombination("5", new String[] { "4", "6", "8" }));
        values.add(new NodeCombination("6", new String[] { "5", "8" }));
        values.add(new NodeCombination("8", new String[] { "5", "6" }));

        try {
            out = driver.withInputKey(key).withInputValues(values).run();
        } catch (IOException ioe) {
            fail();
        }

        List<Pair<Text, Text>> expected = new ArrayList<Pair<Text, Text>>();
        expected.add(new Pair<Text, Text>(new Text("7,4,5"), new Text("")));
        expected.add(new Pair<Text, Text>(new Text("7,5,6,8"), new Text("")));

        assertListEquals(expected, out);
    }   
}