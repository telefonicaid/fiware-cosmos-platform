package es.tid.smartsteps.footfalls.microgrids;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

import org.apache.hadoop.conf.Configuration;

import es.tid.smartsteps.footfalls.microgrids.config.Config;

/**
 *
 * @author dmicol
 */
public class CatchmentsBasedTest {

    protected final Configuration conf;
    protected final String catchments;

    public CatchmentsBasedTest() throws IOException {
        this.conf = Config.load(
                Config.class.getResource("/config.properties").openStream(),
                new Configuration());
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
