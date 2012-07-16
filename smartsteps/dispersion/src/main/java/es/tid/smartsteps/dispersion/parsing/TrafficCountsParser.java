package es.tid.smartsteps.dispersion.parsing;

import static java.util.Arrays.asList;
import java.util.Set;

import com.google.common.collect.ImmutableSet;
import com.google.common.collect.ImmutableSortedSet;
import net.sf.json.JSONArray;
import net.sf.json.JSONException;
import net.sf.json.JSONObject;
import net.sf.json.JSONSerializer;
import org.apache.log4j.Logger;

import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.Counts;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.TrafficCounts;

/**
 * Parses traffic counts vectors for a given cell and day in JSON format.
 *
 * @author logc, sortega
 */
public class TrafficCountsParser extends Parser<TrafficCounts> {

    private static final Logger LOGGER =
            Logger.getLogger(TrafficCountsParser.class);

    public static final String DATE_FIELD_NAME = "date";
    public static final String CELLID_FIELD_NAME = "cellid";
    public static final String LATITUDE_FIELD_NAME = "lat";
    public static final String LONGITUDE_FIELD_NAME = "long";
    public static final Set<String> POIS_NAMES = ImmutableSet.of(
            "pois.BILL", "pois.HOME", "pois.NONE", "pois.OTHER", "pois.WORK");


    private final Set<String> vectorNames;

    public TrafficCountsParser(String[] vectorNames) {
        super(null);
        this.vectorNames = ImmutableSortedSet.<String>naturalOrder()
                .addAll(POIS_NAMES)
                .addAll(asList(vectorNames))
                .build();
    }

    @Override
    public TrafficCounts parse(String value) {
        JSONObject jsonObject;
        try {
            jsonObject = (JSONObject) JSONSerializer.toJSON(value);
        } catch (JSONException ex) {
            LOGGER.warn("Cannot parse traffic count", ex);
            return null;
        }

        TrafficCounts.Builder builder = TrafficCounts.newBuilder();
        builder.setId(jsonObject.getString(CELLID_FIELD_NAME));
        builder.setDate(jsonObject.getString(DATE_FIELD_NAME));
        builder.setLatitude(jsonObject.getDouble(LATITUDE_FIELD_NAME));
        builder.setLongitude(jsonObject.getDouble(LONGITUDE_FIELD_NAME));

        for (String vectorName : this.vectorNames) {
            JSONArray countsArray =
                    (JSONArray) JSONUtil.getProperty(jsonObject, vectorName);
            if (countsArray == null) {
                LOGGER.warn("Vector named " + vectorName + " not found");
                continue;
            }
            Counts.Builder countsBuilder = Counts.newBuilder()
                    .setName(vectorName);
            for (int i = 0; i < countsArray.size(); i++) {
                countsBuilder.addValues(countsArray.getDouble(i));
            }
            builder.addVectors(countsBuilder);
        }
        return builder.build();
    }
}
