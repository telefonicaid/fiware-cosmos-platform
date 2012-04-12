package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.base.Pair;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityArea;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityAreaKey;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;

/**
 *
 * @author losa
 */
public abstract class ActivityAreaUtil implements ProtobufUtil {

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

    public static ActivityArea createArea(int numPos, int difBtss, int difMuns, 
                int difStates, double masscenterUtmX, double masscenterUtmY,
                double radius, double diamAreaInf) {
        ActivityArea.Builder activityAreaVars = ActivityArea.newBuilder()
            .setNumPos(numPos)
            .setDifBtss(difBtss)
            .setDifMuns(difMuns)
            .setDifStates(difStates)
            .setMasscenterUtmX(masscenterUtmX)
            .setMasscenterUtmY(masscenterUtmY)
            .setRadius(radius)
            .setDiamAreaInf(diamAreaInf);
        return activityAreaVars.build();
    }

    public static ActivityAreaKey createKey(long userId, int month,
                boolean isWorkDay) {
        ActivityAreaKey.Builder activityAreaKey = ActivityAreaKey.newBuilder()
            .setUserId(userId)
            .setMonth(month)
            .setIsWorkDay(isWorkDay);
        return activityAreaKey.build();
    }

    public static Cell createCell(long cellId, long placeId, int geoLoc1,
            int geoLoc2, double posX, double posY) {
        Cell.Builder cell = Cell.newBuilder()
                .setCellId(cellId)
                .setPlaceId(placeId)
                .setGeoloc1(geoLoc1)
                .setGeoloc2(geoLoc2)
                .setPosx(posX)
                .setPosy(posY);
        return cell.build();
    }

    public static ProtobufWritable<ActivityArea> wrap(ActivityArea obj) {
        ProtobufWritable<ActivityArea> wrapper =
                ProtobufWritable.newInstance(ActivityArea.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<ActivityAreaKey> wrap(ActivityAreaKey obj) {
        ProtobufWritable<ActivityAreaKey> wrapper =
                ProtobufWritable.newInstance(ActivityAreaKey.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<Cell> wrap(Cell obj) {
        ProtobufWritable<Cell> wrapper = 
                ProtobufWritable.newInstance(Cell.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable createAndWrapArea(int numPos, int difBtss, 
            int difMuns, int difStates, double masscenterUtmX, 
            double masscenterUtmY, double radius, double diamAreaInf) {
        return wrap((createArea(numPos, difBtss, difMuns, difStates, masscenterUtmX,
                    masscenterUtmY, radius, diamAreaInf)));
    }

    public static ProtobufWritable createAndWrapAreaKey(long userId, int month, 
            boolean isWorkDay) {
        ProtobufWritable<ActivityAreaKey> wrapper =
                ProtobufWritable.newInstance(ActivityAreaKey.class);
        wrapper.set(createKey(userId, month, isWorkDay));
        return wrapper;
    }

    public static ProtobufWritable createAndWrapCell(long cellId,
            long placeId, int geoLoc1, int geoLoc2, double posX, double posY) {
        return wrap(createCell(cellId, placeId, geoLoc1, geoLoc2, posX, posY));
    }

    public static Pair<ActivityAreaKey, Cell> parse(String line) {
        String[] fields = line.split(DELIMITER);
        long userId = Long.parseLong(fields[USER_ID_INDEX].trim());
        int month = Integer.parseInt(fields[MONTH_INDEX].trim());
        boolean isWorkDay;
        if (fields[ISWORKDAY_INDEX] == "0") {
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

        ActivityAreaKey key = createKey(userId, month, isWorkDay);
        Cell cell = createCell(cellId, placeId, geoLoc1, geoLoc2, posX, posY);

        return new Pair(key, cell);
    }

    // TODO
    public static String toString(ProtobufWritable<ActivityArea> result) {
        String ans = new String();
        return ans;
    }
}
