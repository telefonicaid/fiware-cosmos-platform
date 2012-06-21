package es.tid.cosmos.platform.injection.server;

import org.apache.commons.cli.ParseException;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;
import org.junit.Before;
import org.junit.Test;

/**
 * @author sortega
 */
public class ServerCommandLineTest {
    private ServerCommandLine instance;

    @Before
    public void setUp() throws Exception {
        this.instance = new ServerCommandLine();
    }

    @Test
    public void emptyCommandLine() throws Exception {
        this.instance.parse(new String[] {});
        assertFalse(this.instance.hasConfigFile());
    }

    @Test
    public void externalShortConfigCommandLine() throws Exception {
        this.instance.parse(new String[] {"-c", "/tmp/test.properties"});
        assertTrue(this.instance.hasConfigFile());
    }

    @Test
    public void externalLongConfigCommandLine() throws Exception {
        this.instance.parse(new String[] {"--config", "/tmp/test.properties"});
        assertTrue(this.instance.hasConfigFile());
    }

    @Test(expected=ParseException.class)
    public void unexpectedOptionsCommandLine() throws Exception {
        this.instance.parse(new String[] {"-x"});
    }
}
