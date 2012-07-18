package es.tid.smartsteps.footfalls.catchments;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

import es.tid.smartsteps.footfalls.ConfigurationTestBase;
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.Catchment;
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.Catchments;
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.TopCell;

/**
 *
 * @author dmicol
 */
public class CatchmentsTestBase extends ConfigurationTestBase {

    public static final String SAMPLE_DATE = "20120504";

    protected final String catchments;

    public CatchmentsTestBase() {
        try {
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
                    } catch (IOException ignored) {
                        // ignored
                    }
                }
            }
        } catch (IOException ex) {
            throw new ExceptionInInitializerError(ex);
        }
    }

    protected static Catchments singletonCatchment(String topLevelId, int hour,
                                                   String cellId, double count) {
        Catchment.Builder catchment = Catchment.newBuilder()
                .setHour(hour)
                .addTopCells(TopCell.newBuilder()
                        .setId(cellId)
                        .setCount(count)
                        .setLatitude(1d)
                        .setLongitude(2d));
        return Catchments.newBuilder()
                .setId(topLevelId)
                .setDate(SAMPLE_DATE)
                .setLatitude(0d)
                .setLongitude(0d)
                .addCatchments(catchment)
                .build();
    }
}
