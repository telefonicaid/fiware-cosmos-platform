package es.tid.smartsteps.footfalls.microgrids;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

/**
 *
 * @author dmicol
 */
public class CatchmentsBasedTest extends ConfigurationBasedTest {

    protected final String catchments;

    public CatchmentsBasedTest() throws IOException {
        BufferedReader reader = null;
        try {
            reader = new BufferedReader(
                    new FileReader("src/test/resources/catchments.json"));
            String contents = "";
            while (reader.ready()) {
                contents += reader.readLine() + "\n";
            }
            this.catchments = contents;
        } finally {
            if (reader != null) {
                try {
                    reader.close();
                } catch (IOException ex2) {
                }
            }
        }
    }
}
