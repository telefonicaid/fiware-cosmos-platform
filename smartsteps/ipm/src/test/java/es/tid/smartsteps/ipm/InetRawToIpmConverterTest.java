package es.tid.smartsteps.ipm;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author dmicol
 */
public class InetRawToIpmConverterTest {
    private InetRawToIpmConverter instance;
    
    @Before
    public void setUp() {
        this.instance = new InetRawToIpmConverter();
    }

    @Test
    public void testConvert() {
        String output = this.instance.convert("0|1|2|3|4|5|6|7|"
                                              + "8|9|10|11|12|13");
        assertEquals(output, "0|1|40b244112641dd78dd4f93b6c9190dd46e0099194d5a4"
                + "4257b7efad6ef9ff4683da1eda0244448cb343aa688f5d3efd7314dafe58"
                + "0ac0bcbf115aeca9e8dc114|3|4||06df05371981a237d0ed11472fae7c9"
                + "4c9ac0eff1d05413516710d17b10a4fb6f4517bda4a695f02d0a73dd4db5"
                + "43b4653df28f5d09dab86f92ffb9b86d01e25|6|7|8|9|10|11|12|13");
    }
}
