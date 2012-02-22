package es.tid.ps.mobility.data;

import java.util.ArrayList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.ps.mobility.data.MobProtocol.Bts;
import es.tid.ps.mobility.data.MobProtocol.GMTPoint;
import es.tid.ps.mobility.data.MobProtocol.UTMPoint;

/**
 *
 * @author sortega
 */
public abstract class BtsUtil implements ProtobufUtil {
    public static Bts create(long placeId, double posx, double posy,
            double lat, double lon, double area, Iterable<UTMPoint> utmPolygon,
            Iterable<GMTPoint> gmtPolygon, Iterable<Long> adjBts) {
        Bts.Builder bts = Bts.newBuilder()
                .setPlaceId(placeId)
                .setPosx(posx)
                .setPosy(posy)
                .setLat(lat)
                .setLon(lon)
                .setArea(area)
                .addAllUtmPolygon(utmPolygon)
                .addAllGmtPolygon(gmtPolygon)
                .addAllAdjBts(adjBts);
        return bts.build();
    }

    public static ProtobufWritable<Bts> createAndWrap(long placeId, double posx,
            double posy, double lat, double lon, double area,
            Iterable<UTMPoint> utmPolygon, Iterable<GMTPoint> gmtPolygon,
            Iterable<Long> adjBts) {
        ProtobufWritable<Bts> wrapper =
                ProtobufWritable.newInstance(Bts.class);
        wrapper.set(create(placeId, posx, posy, lat, lon, area, utmPolygon,
                gmtPolygon, adjBts));
        return wrapper;
    }

    /**
     * Builds a polygon from the parameters
     * @param coords posx1, posy1, posx2, posy2...
     * @return An UTMPoint polygon
     */
    public static Iterable<UTMPoint> utmPolygon(double... coords) {
        if (coords.length % 2 != 0) {
            throw new IllegalArgumentException("Odd number of coordinates");
        }

        List<UTMPoint> points = new ArrayList<UTMPoint>();
        for (int point = 0; point * 2 < coords.length; point++) {
            points.add(UTMPoint.newBuilder()
                    .setPosx(coords[point * 2])
                    .setPosy(coords[point * 2 + 1])
                    .build());
        }
        return points;
    }

    /**
     * Builds a polygon from the parameters
     * @param coords lat1, lon1, lat2, lon2...
     * @return An GMTPoint polygon
     */
    public static Iterable<GMTPoint> gmtPolygon(double... coords) {
        if (coords.length % 2 != 0) {
            throw new IllegalArgumentException("Odd number of coordinates");
        }

        List<GMTPoint> points = new ArrayList<GMTPoint>();
        for (int point = 0; point * 2 < coords.length; point++) {
            points.add(GMTPoint.newBuilder()
                    .setLat(coords[point * 2])
                    .setLon(coords[point * 2 + 1])
                    .build());
        }
        return points;
    }
}