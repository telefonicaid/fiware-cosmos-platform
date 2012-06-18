package es.tid.smartsteps.ipm;

import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.fail;

/**
 *
 * @author dmicol
 */
public class CrmRawToIpmConverterTest {
    private CrmRawToIpmConverter instance;
    
    @Before
    public void setUp() {
        this.instance = new CrmRawToIpmConverter();
    }

    @Test
    public void testConvert() {
        String output = null;
        try {
            output = this.instance.convert("0|1|2|3|4|5|6|7|8|9|10|"
                                                  + "11|12|13|14|15|16|17");
        } catch (ParseException e) {
            fail(String.format("unexpected parsing error: %s", e.getCause()));
        }
        assertEquals(output, "31bca02094eb78126a517b206a88c73cfa9ec6f704c7030d1"
                + "8212cace820f025f00bf0ea68dbf3f3a5436ca63b53bf7bf80ad8d5de7d8"
                + "359d0b7fed9dbc3ab99|4dff4ea340f0a823f15d3f4f01ab62eae0e5da57"
                + "9ccb851f8db9dfe84c58b2b37b89903a740e1ee172da793a6e79d560e5f7"
                + "f9bd058a12a280433ed6fa46510a|2|3|4|5|6|7|8|9|10|11|5aadb4552"
                + "0dcd8726b2822a7a78bb53d794f557199d5d4abdedd2c55a4bd6ca736076"
                + "05c558de3db80c8e86c3196484566163ed1327e82e8b6757d1932113cb8|"
                + "413f2ba78c7ed4ccefbe0cc4f51d3eb5cb15f13fec999de4884be9250767"
                + "46663aa5d34476a3df4a8729fd8eea01defa4f3f66e99bf943f4d84382d6"
                + "4bbbfa9e||14|15|16|17");
    }

    @Test
    public void testConvertInvalidInput() {
        try {
            this.instance.convert("Hello world!");
            fail(String.format("expected parsing exception not thrown"));
        } catch (ParseException e) {
        }
    }

    @Test
    public void testConvertInvalidFieldCount() {
        try {
            this.instance.convert("0|1|2|3|4|5|6|7|8|9|10|");
            fail(String.format("expected parsing exception not thrown"));
        } catch (ParseException e) {
        }
    }
}
