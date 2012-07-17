package es.tid.smartsteps.footfalls.microgrids.parsing;

import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol;
import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.Catchment;
import net.sf.json.JSONArray;
import net.sf.json.JSONException;
import net.sf.json.JSONObject;
import net.sf.json.JSONSerializer;
import org.apache.log4j.Logger;

import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.Catchments;
import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.TopCell;

/**
 * Parses catchments for a given cell and day in JSON format.
 *
 * @author dmicol
 */
public class CatchmentsParser extends Parser<Catchments> {

    private static final Logger LOGGER = Logger.getLogger(CatchmentsParser.class);

    public static final String TOP_LEVEL_CELLID_FIELD_NAME = "topLevelCellid";
    public static final String DATE_FIELD_NAME = "date";
    public static final String LATITUDE_FIELD_NAME = "lat";
    public static final String LONGITUDE_FIELD_NAME = "long";
    public static final String CATCHMENTS_FIELD_NAME = "catchments";
    public static final String HOUR_FIELD_NAME = "hour";
    public static final String TOP_CELLS_FIELD_NAME = "topCells";
    public static final String CELLID_FIELD_NAME = "cellId";
    public static final String COUNT_FIELD_NAME = "count";
    public static final String LONLAT_FIELD_NAME = "lonlat";

    public CatchmentsParser() {
        super(null);
    }

    @Override
    public Catchments parse(String value) {
        JSONObject jsonObject;
        try {
            jsonObject = (JSONObject) JSONSerializer.toJSON(value);
        } catch (JSONException ex) {
            LOGGER.warn("Cannot parse catchments ", ex);
            return null;
        }

        Catchments.Builder builder = Catchments.newBuilder();
        builder.setId(jsonObject.getString(TOP_LEVEL_CELLID_FIELD_NAME));
        builder.setDate(jsonObject.getString(DATE_FIELD_NAME));
        builder.setLatitude(jsonObject.getDouble(LATITUDE_FIELD_NAME));
        builder.setLongitude(jsonObject.getDouble(LONGITUDE_FIELD_NAME));
        JSONArray catchmentsArray = jsonObject.getJSONArray(
                CATCHMENTS_FIELD_NAME);
        for (int i = 0; i < catchmentsArray.size(); i++) {
            JSONObject catchment = catchmentsArray.getJSONObject(i);
            Catchment.Builder catchmentBuilder = Catchment.newBuilder();
            catchmentBuilder.setHour(catchment.getInt(HOUR_FIELD_NAME));
            JSONArray topCells = catchment.getJSONArray(TOP_CELLS_FIELD_NAME);
            for (int j = 0; j < topCells.size(); j++) {
                JSONObject topCell = topCells.getJSONObject(j);
                TopCell.Builder topCellBuilder = TopCell.newBuilder();
                topCellBuilder.setId(topCell.getString(CELLID_FIELD_NAME));
                topCellBuilder.setCount(topCell.getInt(COUNT_FIELD_NAME));
                JSONArray lonlat = topCell.getJSONArray(LONLAT_FIELD_NAME);
                topCellBuilder.setLongitude(lonlat.getLong(0));
                topCellBuilder.setLatitude(lonlat.getLong(1));
                catchmentBuilder.addTopCells(topCellBuilder);
            }
            builder.addCatchments(catchmentBuilder);
        }

        return builder.build();
    }
}
