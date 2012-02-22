package es.tid.ps.mobility.parsing;

import java.util.ArrayList;
import java.util.List;

import static org.junit.Assert.assertEquals;
import org.junit.Test;

import es.tid.ps.mobility.data.BaseProtocol.Date;
import es.tid.ps.mobility.data.BaseProtocol.Time;
import es.tid.ps.mobility.data.Client;
import es.tid.ps.mobility.data.GeoLocation;
import es.tid.ps.mobility.data.GeoLocationContainer;
import es.tid.ps.mobility.data.MobProtocol.GLEvent;
import es.tid.ps.mobility.data.Utils;

public class ClientTest {
    @SuppressWarnings("deprecation")
    @Test
    public void testCalculateGeoLocationsNoInfoGeolocations() {
        Client client = new Client();
        List<GLEvent> listGLEvent = new ArrayList<GLEvent>();
        List<Integer> noInfoGeolocationsExpected = new ArrayList<Integer>();
        List<Integer> noInfoGeolocationsActual = null;
        GLEvent.Builder glEvent = GLEvent.newBuilder();
        glEvent.setUserId(0);
        glEvent.setPlaceId(0);
        Date.Builder date = Date.newBuilder();
        date.setYear(2012);
        date.setMonth(2);
        date.setDay(4);
        date.setWeekday(7);
        glEvent.setDate(date);
        //GLEvent glEvent = new GLEvent(0, 0,
        //		new Date((2012 - 1900), 1, 4, 23, 5));
        Time.Builder time = Time.newBuilder();
        time.setHour(23);
        time.setMinute(5);
        time.setSeconds(0);
        glEvent.setTime(time);

        for (int position = 0; position < 168; position++) {
            noInfoGeolocationsExpected.add(position, new Integer(0));
        }

        noInfoGeolocationsExpected.set(167, new Integer(1));

        listGLEvent.add(glEvent.build());
        client.calculateGeoLocations(listGLEvent.iterator());
        noInfoGeolocationsActual = client.getNoInfoGeolocations();

        assertEquals(noInfoGeolocationsExpected, noInfoGeolocationsActual);

    }

    @SuppressWarnings("deprecation")
    @Test
    public void testCalculateGeoLocationsGeolocations() {
        Client client = new Client();
        List<GLEvent> listGLEvent = new ArrayList<GLEvent>();
        GeoLocationContainer geoLocationsExpected = new GeoLocationContainer();

        GLEvent.Builder glEvent = GLEvent.newBuilder();
        glEvent.setUserId(0);
        glEvent.setPlaceId(333);
        //GLEvent glEvent = new GLEvent(0, 333, new Date((2012 - 1900), 1, 4, 23,
        //		5));
        Date.Builder date = Date.newBuilder();
        date.setYear(2012);
        date.setMonth(2);
        date.setDay(4);
        date.setWeekday(7);
        glEvent.setDate(date);
        Time.Builder time = Time.newBuilder();
        time.setHour(23);
        time.setMinute(5);
        time.setSeconds(0);
        glEvent.setTime(time);
        listGLEvent.add(glEvent.build());

        client.calculateGeoLocations(listGLEvent.iterator());
        GeoLocationContainer geoLocationsActual = client.getGeoLocations();

        geoLocationsExpected.incrementGeoLocation(new GeoLocation(333,
                (byte) 7, (byte) 23));

        assertEquals(geoLocationsExpected, geoLocationsActual);
    }

    @Test
    public void testGetPosListNoInfoGeolocationsAll() {

        assertEquals(167,
                Utils.getPosListNoInfoGeolocations((byte) 7, (byte) 23));

    }

    @Test
    public void testGetPosListNoInfoGeolocationsOutOfRangeD() {

        assertEquals(-1,
                Utils.getPosListNoInfoGeolocations((byte) 8, (byte) 21));

    }

    @Test
    public void testGetPosListNoInfoGeolocationsOutOfRangeH() {

        assertEquals(-1,
                Utils.getPosListNoInfoGeolocations((byte) 6, (byte) 25));

    }
}
