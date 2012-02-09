package es.tid.mapreduce.mobility2.data;

import static es.tid.mapreduce.mobility2.data.RepresentVector.generateRepresentVector;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;

import junit.framework.TestCase;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.BlockJUnit4ClassRunner;


@RunWith(BlockJUnit4ClassRunner.class)
public class RepresentVectorTest extends TestCase{
    @Test
    public void generateRepresentVectorTest() throws IOException {        
        InputStream fileInput = new FileInputStream("src/test/resources/represent_vector.dat");
        RepresentVector actual = generateRepresentVector(fileInput);
        
        Vector vector = new Vector(96);
        vector.set(0, 0);
        vector.set(1, 1);           
        RepresentVector expected = new RepresentVector("test", vector );
        
        assertEquals(expected, actual);
    }
}
