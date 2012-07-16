package es.tid.cosmos.base.util;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author dmicol
 */
public class ArgumentParserTest {
    private ArgumentParser instance;

    @Before
    public void setUp() {
        this.instance = new ArgumentParser();
    }

    @Test
    public void testStrings() {
        this.instance.parse(new String[] { "--arg1", "--arg2=false" });
        assertEquals("true", this.instance.getString("arg1"));
        assertEquals("false", this.instance.getString("arg2"));
    }

    @Test
    public void testBooleans() {
        this.instance.parse(new String[] { "--arg1", "--arg2=false" });
        assertEquals(true, this.instance.getBoolean("arg1"));
        assertEquals(false, this.instance.getBoolean("arg2"));
    }

    @Test
    public void testMissingAndNotRequired() {
        this.instance.parse(new String[] { "--arg1", "--arg2=false" });
        assertEquals(false, this.instance.has("arg3"));
    }

    @Test(expected=IllegalArgumentException.class)
    public void testMissingAndRequired() {
        this.instance.parse(new String[] { "--arg1", "--arg2=false" });
        assertEquals(false, this.instance.has("arg3", true));
    }

    @Test(expected=IllegalArgumentException.class)
    public void testInvalid() {
        this.instance.parse(new String[] { "--arg1", "--arg2=false=true" });
    }
}
