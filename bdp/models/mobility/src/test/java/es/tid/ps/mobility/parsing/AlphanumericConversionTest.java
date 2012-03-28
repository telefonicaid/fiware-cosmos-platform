package es.tid.ps.mobility.parsing;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotSame;
import org.junit.Test;

/**
 * User: masp20
 * Date: 26-ene-2012
 * Time: 16:20:31
 */
public class AlphanumericConversionTest {

    @Test
    public void testCorrect() {
        final String start = "ABCD1234";

        final long codedString = ParserAlphanumeric.gstCodedStrToLong(start, ParserAlphanumeric.MAX_FIELD_DIGIT);
        final String decoded = ParserAlphanumeric.gstDecodeAphaIntToStr(codedString);

        assertEquals("encode/decode Fail", start, decoded);
    }


    @Test
    public void testFail() {
        final String start = "ABCD1234";

        final long codedString = ParserAlphanumeric.gstCodedStrToLong(start, ParserAlphanumeric.MAX_FIELD_DIGIT);
        final String decoded = ParserAlphanumeric.gstDecodeAphaIntToStr(codedString);

        assertNotSame("encode/decode Fail", start + "_FAIL", decoded);
    }
}
