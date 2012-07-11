package es.tid.cosmos.mobility.parsing;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.generated.BaseProtocol;

/**
 * @author sortega
 */
public class DateParserTest {

    private DateParser instance;

    @Before
    public void setUp() throws Exception {
        this.instance = new DateParser("yyyy-mm-dd");
    }

    @Test(expected = IllegalArgumentException.class)
    public void shouldRequireAllTheFields() throws Exception {
        new DateParser("mmdd");
    }

    @Test(expected = IllegalArgumentException.class)
    public void shouldThrowOnRepeatedFields() throws Exception {
        new DateParser("yyyy-mm-dd-yy");
    }

    @Test
    public void shouldParseValidDate() throws Exception {
        BaseProtocol.Date date = this.instance.parse("2012-07-10");
        assertNotNull(date);
        assertEquals(2012, date.getYear());
        assertEquals(7, date.getMonth());
        assertEquals(10, date.getDay());
        assertEquals(2, date.getWeekday());
    }

    @Test(expected = IllegalArgumentException.class)
    public void shouldThrowOnShortInput() throws Exception {
        this.instance.parse("2012");
    }

    @Test(expected = IllegalArgumentException.class)
    public void shouldThrowOnNonIntegerFields() throws Exception {
        this.instance.parse("2012-mm-10");
    }

    @Test
    public void shouldIgnoreNonFieldCharacters() throws Exception {
        BaseProtocol.Date date = this.instance.parse("1912/06/23");
        assertNotNull(date);
        assertEquals(1912, date.getYear());
        assertEquals(6, date.getMonth());
        assertEquals(23, date.getDay());
    }

    @Test
    public void shouldBeCaseInsensitive() throws Exception {
        this.instance = new DateParser("YYYYMMDD");
        BaseProtocol.Date date = this.instance.parse("19460214");
        assertNotNull(date);
        assertEquals(1946, date.getYear());
        assertEquals(2, date.getMonth());
        assertEquals(14, date.getDay());
    }
}
