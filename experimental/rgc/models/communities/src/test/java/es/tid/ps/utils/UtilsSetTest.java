package es.tid.ps.utils;

import java.util.Set;
import java.util.TreeSet;

import junit.framework.TestCase;
import org.junit.Test;

public class UtilsSetTest extends TestCase {

    @Test
    public void testGenerateIntersection() {
        Set<Integer> out;
        Set<Integer> a = new TreeSet<Integer>();
        Set<Integer> b = new TreeSet<Integer>();
        Set<Integer> expected = new TreeSet<Integer>();

        a.add(1);
        a.add(2);
        b.add(1);
        b.add(3);
        expected.add(1);

        out = UtilsSet.generateIntersection(a, b);

        assertEquals(expected, out);
    }

    @Test
    public void testGenerateDifference() {
        Set<Integer> out;
        Set<Integer> a = new TreeSet<Integer>();
        Set<Integer> b = new TreeSet<Integer>();
        Set<Integer> expected = new TreeSet<Integer>();

        a.add(1);
        a.add(2);
        b.add(1);
        b.add(3);
        expected.add(2);

        out = UtilsSet.generateDifference(a, b);

        assertEquals(expected, out);
    }
    
    @Test
    public void testSymmetricGenerateDifference() {
        Set<Integer> out;
        Set<Integer> a = new TreeSet<Integer>();
        Set<Integer> b = new TreeSet<Integer>();
        Set<Integer> expected = new TreeSet<Integer>();

        a.add(1);
        a.add(2);
        b.add(1);
        b.add(3);
        expected.add(2);
        expected.add(3);

        out = UtilsSet.generateSymmetricDifference(a, b);

        assertEquals(expected, out);
    }

    @Test
    public void testGenerateUnion() {
        Set<Integer> out;
        Set<Integer> a = new TreeSet<Integer>();
        Set<Integer> b = new TreeSet<Integer>();
        Set<Integer> expected = new TreeSet<Integer>();

        a.add(1);
        a.add(2);
        b.add(1);
        b.add(3);
        expected.add(1);
        expected.add(2);
        expected.add(3);

        out = UtilsSet.generateUnion(a, b);

        assertEquals(expected, out);
    }

}
