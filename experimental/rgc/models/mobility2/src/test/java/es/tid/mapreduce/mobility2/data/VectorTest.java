package es.tid.mapreduce.mobility2.data;


import junit.framework.TestCase;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.BlockJUnit4ClassRunner;


@RunWith(BlockJUnit4ClassRunner.class)
public class VectorTest extends TestCase {

    @Test
    public void normalize() {        
        Vector actual = new Vector(96);
        actual.set(0, 1);
        actual.set(1, 1);
        
        Vector expected = new Vector(96);
        expected.set(0, 0.5);
        expected.set(1, 0.5);        
        expected.set(2, 0);
        
        actual.normalize();        
        assertEquals(expected, actual);      
    }
    
    @Test
    public void preNormalizeByMobility() {        
        Vector actual = new Vector(96);
        actual.set(0, 4);
        actual.set(95, 1);
        
        Vector expected = new Vector(96);
        expected.set(0, 1);
        expected.set(95, 1);
        
        actual.preNormalizeByMobility();        
        assertEquals(expected, actual);      
    }
    
    @Test
    public void pearsonCorrelation() {        
        Vector initial = new Vector(96);
        initial.set(0, 4);
        initial.set(95, 1);
        
        Vector target = new Vector(96);
        target.set(0, 1);
        target.set(95, 1);
        
        double actual = initial.getPearsonCorrelation(target);      
        double expected = 0.8550883606637538;
        assertEquals(new Double(expected), new Double(actual));      
    }
    
    @Test
    public void increment() {        
        Vector actual = new Vector(96);
        actual.set(0, 4);
        actual.set(95, 1);
        
        Vector expected = new Vector(96);
        expected.set(0, 5);
        expected.set(95, 1);
        
        actual.incrementPosition(0);
        
        assertEquals(expected, actual);    
    }
}
