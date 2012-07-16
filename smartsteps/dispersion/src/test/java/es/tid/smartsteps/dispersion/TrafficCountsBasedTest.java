package es.tid.smartsteps.dispersion;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

import org.apache.hadoop.conf.Configuration;

import es.tid.smartsteps.dispersion.config.Config;

/**
 *
 * @author dmicol
 */
public class TrafficCountsBasedTest {

    protected final Configuration conf;
    protected final String trafficCounts;

    public TrafficCountsBasedTest() throws IOException {
        this.conf = Config.load(
                Config.class.getResource("/config.properties").openStream(),
                new Configuration());
        BufferedReader reader = null;
        try {
            reader = new BufferedReader(
                    new FileReader("src/test/resources/trafficdata.json"));
            this.trafficCounts = reader.readLine();
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
