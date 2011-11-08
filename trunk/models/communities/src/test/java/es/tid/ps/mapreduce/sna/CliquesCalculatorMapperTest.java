package es.tid.ps.mapreduce.sna;

import static org.apache.hadoop.mrunit.testutil.ExtendedAssert.assertListEquals;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import junit.framework.TestCase;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.BlockJUnit4ClassRunner;

/**
 * Test cases for the Cliques Calculator mapper.
 * 
 * @author rgc@tid.es
 */
@RunWith(BlockJUnit4ClassRunner.class)
public class CliquesCalculatorMapperTest extends TestCase {
    private Mapper<Text, ArrayListWritable, Text, NodeCombination> mapper;
    private MapDriver<Text, ArrayListWritable, Text, NodeCombination> driver;

    @Before
    public void setUp() {
        mapper = new CliquesCalculatorMapper();
        driver = new MapDriver<Text, ArrayListWritable, Text, NodeCombination>(
                mapper);
    }

    @Test
    public void testMapNodeCombinations() {
        List<Pair<Text, NodeCombination>> out = null;

        String[] vv = { new String("1"), new String("3"), new String("4") };
        ArrayListWritable val = new ArrayListWritable(vv);

        try {
            out = driver.withInputKey(new Text("2")).withInputValue(val).run();
        } catch (IOException ioe) {
            fail();
        }
        List<Pair<Text, NodeCombination>> expected = new ArrayList<Pair<Text, NodeCombination>>();
        expected.add(new Pair<Text, NodeCombination>(new Text("1"),
                new NodeCombination("2", new String[] { "3", "4" })));
        expected.add(new Pair<Text, NodeCombination>(new Text("3"),
                new NodeCombination("2", new String[] { "1", "4" })));
        expected.add(new Pair<Text, NodeCombination>(new Text("4"),
                new NodeCombination("2", new String[] { "1", "3" })));

        assertListEquals(expected, out);
    }
}
