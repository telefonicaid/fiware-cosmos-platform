package es.tid.smartsteps.ipm;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.Charset;

import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.assertArrayEquals;

/**
 *
 * @author dmicol
 * @author apv
 */
public class InetRawToIpmConverterTest {

    private InetRawToIpmConverter instance;
    private Charset charset;

    @Before
    public void setUp() {
        this.charset = Charset.forName("UTF-8");
        this.instance = new InetRawToIpmConverter("|", this.charset);
    }

    @Test
    public void testConvert() throws IOException, ParseException {
        InputStream input = new ByteArrayInputStream
                ("0|1|2|3|4|5|6|7|8|9|10|11|12|13".getBytes(this.charset));

        ByteArrayOutputStream output = new ByteArrayOutputStream();
        this.instance.convert(input, output);
        assertArrayEquals(output.toByteArray(),
                ("0|1|40b244112641dd78dd4f93b6c9190dd46e0099194d5a4" +
                        "4257b7efad6ef9ff4683da1eda0244448cb343aa688f5d3efd7314dafe58" +
                        "0ac0bcbf115aeca9e8dc114|3|4||06df05371981a237d0ed11472fae7c9" +
                        "4c9ac0eff1d05413516710d17b10a4fb6f4517bda4a695f02d0a73dd4db5" +
                        "43b4653df28f5d09dab86f92ffb9b86d01e25|6|7|8|9|10|11|12|13").
                        getBytes(this.charset));
    }

    @Test(expected = ParseException.class)
    public void testConvertInvalidInput() throws IOException, ParseException {
        this.instance.convert(
                new ByteArrayInputStream("Hello world!".getBytes()),
                new ByteArrayOutputStream());
    }

    @Test(expected = ParseException.class)
    public void testConvertInvalidFieldCount() throws IOException, ParseException {
        this.instance.convert(
                new ByteArrayInputStream("0|1|2|3|4|5|6|7|8|9|10".getBytes()),
                new ByteArrayOutputStream());
    }
}
