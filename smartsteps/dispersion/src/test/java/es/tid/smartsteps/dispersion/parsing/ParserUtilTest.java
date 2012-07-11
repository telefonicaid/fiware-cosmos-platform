package es.tid.smartsteps.dispersion.parsing;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;
import org.junit.Test;

/**
 *
 * @author sortega
 */
public class ParserUtilTest {

    @Test
    public void isQuotedTest() {
        assertTrue(ParserUtil.isQuoted("\"Quoted\""));
        assertFalse(ParserUtil.isQuoted("Not quoted\""));
        assertFalse(ParserUtil.isQuoted("\"Not quoted"));
        assertFalse(ParserUtil.isQuoted("Not quoted"));
    }

    @Test
    public void safeUnquote() {
        assertEquals("quoted text", ParserUtil.safeUnquote("\"quoted text\""));
        assertEquals("unquoted text", ParserUtil.safeUnquote("unquoted text"));
    }
}
