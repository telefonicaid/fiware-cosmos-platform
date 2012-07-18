package es.tid.smartsteps.footfalls.trafficcounts;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

import es.tid.smartsteps.footfalls.ConfigurationBasedTest;

/**
 *
 * @author dmicol
 */
public class TrafficCountsBasedTest extends ConfigurationBasedTest {

    protected final String trafficCounts;

    public TrafficCountsBasedTest() throws IOException {
        BufferedReader reader = null;
        try {
            reader = new BufferedReader(
                    new FileReader("src/test/resources/trafficdata.json"));
            String contents = "";
            while (reader.ready()) {
                contents += reader.readLine() + "\n";
            }
            this.trafficCounts = contents;
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
