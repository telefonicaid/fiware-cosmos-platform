package es.tid.smartsteps.footfalls.microgrids.parsing;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import es.tid.smartsteps.footfalls.microgrids.CatchmentsBasedTest;
import java.io.IOException;

/**
 *
 * @author dmicol
 */
public class CatchmentsParserTest extends CatchmentsBasedTest {

    CatchmentsParser instance;

    public CatchmentsParserTest() throws IOException {}

    @Before
    public void setUp() {
        this.instance = new CatchmentsParser();
    }

    @Test
    public void testParse() {
        this.instance.parse(this.catchments);
    }
}
