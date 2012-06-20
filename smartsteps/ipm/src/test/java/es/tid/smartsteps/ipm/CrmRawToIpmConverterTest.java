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
public class CrmRawToIpmConverterTest {
    private CrmRawToIpmConverter instance;
    private Charset charset;
    
    @Before
    public void setUp() {
        this.charset = Charset.forName("UTF-8");
        this.instance = new CrmRawToIpmConverter("|", this.charset);
    }

    @Test
    public void testConvert() throws IOException, ParseException {
        InputStream input = new ByteArrayInputStream
                ("0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17".getBytes(
                        this.charset));

        ByteArrayOutputStream output = new ByteArrayOutputStream();
        this.instance.convert(input, output);
        assertArrayEquals(output.toByteArray(),
                ("31bca02094eb78126a517b206a88c73cfa9ec6f704c7030d1" +
                 "8212cace820f025f00bf0ea68dbf3f3a5436ca63b53bf7bf80ad8d5de7d8" +
                 "359d0b7fed9dbc3ab99|4dff4ea340f0a823f15d3f4f01ab62eae0e5da57" +
                 "9ccb851f8db9dfe84c58b2b37b89903a740e1ee172da793a6e79d560e5f7" +
                 "f9bd058a12a280433ed6fa46510a|2|3|4|5|6|7|8|9|10|11|5aadb4552" +
                 "0dcd8726b2822a7a78bb53d794f557199d5d4abdedd2c55a4bd6ca736076" +
                 "05c558de3db80c8e86c3196484566163ed1327e82e8b6757d1932113cb8|" +
                 "413f2ba78c7ed4ccefbe0cc4f51d3eb5cb15f13fec999de4884be9250767" +
                 "46663aa5d34476a3df4a8729fd8eea01defa4f3f66e99bf943f4d84382d6" +
                 "4bbbfa9e||14|15|16|17").getBytes(this.charset));
    }

    @Test(expected = ParseException.class)
    public void testConvertInvalidInput() throws IOException, ParseException {
        this.instance.convert(
                new ByteArrayInputStream("Hello world!".getBytes()),
                new ByteArrayOutputStream());
    }

    @Test(expected = ParseException.class)
    public void testConvertInvalidFieldCount()
            throws IOException, ParseException {
        this.instance.convert(
                new ByteArrayInputStream("0|1|2|3|4|5|6|7|8|9|10|".getBytes()),
                new ByteArrayOutputStream());
    }
}
