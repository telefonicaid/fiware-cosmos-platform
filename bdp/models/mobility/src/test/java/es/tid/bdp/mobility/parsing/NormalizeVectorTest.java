package es.tid.bdp.mobility.parsing;

import java.util.ArrayList;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.bdp.mobility.data.Utils;

public class NormalizeVectorTest {
    private ArrayList<Double> vector;
    private ArrayList<Double> result;
    private ArrayList<Double> expected;

    @Before
    public void setUp() throws Exception {
        vector = new ArrayList<Double>();
        result = new ArrayList<Double>();
        expected = new ArrayList<Double>();
    }

    @Test
    public void emptyTest() {
        result = Utils.normalizationOfVector(vector);
        assertEquals(expected, result);
    }

    @Test
    public void partTest() {
        // Input
        vector.add(0.0);
        vector.add(1.0);
        vector.add(2.0);
        vector.add(3.0);
        vector.add(4.0);
        vector.add(5.0);
        // Expected output
        expected.add(0.0);
        expected.add(0.06666666666666667);
        expected.add(0.13333333333333333);
        expected.add(0.19999999999999998);
        expected.add(0.26666666666666667);
        expected.add(0.33333333333333333);

        result = Utils.normalizationOfVector(vector);
        assertEquals(expected, result);
    }

    @Test
    public void partFridayTest() {
        // Input
        for (int i = 0; i < 24; i++) {
            vector.add(0.0);
            expected.add(0.0);
        }
        vector.add(100.0);
        vector.add(200.0);
        vector.add(300.0);
        // Expected output
        expected.add(0.16666666666666669);
        expected.add(0.33333333333333337);
        expected.add(0.5);

        result = Utils.normalizationOfVector(vector);
        assertEquals(expected, result);
    }

    @Test
    public void completeTest() {
        for (int i = 0; i < 96; i++) {
            vector.add((double) i % 6);
        }
        result = Utils.normalizationOfVector(vector);
        expected = result; // TODO Es test esta pendiente de realizar
        assertEquals(expected, result);
    }
}
