package es.tid.cosmos.mobility.parsing;

import com.twitter.elephantbird.util.Pair;

import es.tid.cosmos.mobility.data.ActivityAreaUtil;
import es.tid.cosmos.mobility.data.ActivityAreaKeyUtil;
import es.tid.cosmos.mobility.data.CellUtil;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityArea;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityAreaKey;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;

/**
 *
 * @author losa
 */
public class ActivityAreaParser extends Parser {
    private static final String DELIMITER = " ";

    private static final int USER_ID_INDEX   = 0;
    private static final int MONTH_INDEX     = 1;
    private static final int CELLID_INDEX    = 2;
    private static final int ISWORKDAY_INDEX = 3;
    private static final int PLACEID_INDEX   = 4;
    private static final int GEOLOC1_INDEX   = 5;
    private static final int GEOLOC2_INDEX   = 6;
    private static final int POSX_INDEX      = 7;
    private static final int POSY_INDEX      = 8;

    public ActivityAreaParser(String line) {
        super(line, DELIMITER);
    }

    @Override
    public Pair<ActivityAreaKey, Cell> parse() {
        String[] fields = this.line.split(DELIMITER);
        long userId = Long.parseLong(fields[USER_ID_INDEX].trim());
        int month = Integer.parseInt(fields[MONTH_INDEX].trim());
        boolean isWorkDay;
        if (fields[ISWORKDAY_INDEX].equals("0")) {
            isWorkDay = true;
        } else {
            isWorkDay = false;
        }
        long cellId = Long.parseLong(fields[CELLID_INDEX].trim());
        long placeId = Long.parseLong(fields[PLACEID_INDEX].trim());
        int geoLoc1 = Integer.parseInt(fields[GEOLOC1_INDEX].trim());
        int geoLoc2 = Integer.parseInt(fields[GEOLOC2_INDEX].trim());
        double posX = Double.parseDouble(fields[POSX_INDEX].trim());
        double posY = Double.parseDouble(fields[POSY_INDEX].trim());

        ActivityAreaKey key = ActivityAreaKeyUtil.create(userId, month,
                                                         isWorkDay);
        Cell cell = CellUtil.create(cellId, placeId, geoLoc1,
                                    geoLoc2, posX, posY);

        return new Pair(key, cell);
    }
}
