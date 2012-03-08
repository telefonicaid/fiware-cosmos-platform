package es.tid.ps.mobility.parsing;

import java.util.ArrayList;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.ps.mobility.data.Client;
import es.tid.ps.mobility.data.GeoLocation;
import es.tid.ps.mobility.data.GeoLocationContainer;

public class GeoLocationRepresTest {

    private Client client;
    private GeoLocationContainer result;
    ArrayList<Long> expected;
    GeoLocation locat;

    @Before
    public void setUp() {
        client = new Client();
        result = new GeoLocationContainer();
        expected = new ArrayList<Long>();
    }

    @Test
    public void testNodeFiltered() {
        // Input
        locat = new GeoLocation(1010, (byte) 6, (byte) 21);
        result.putGeoLocation(locat, 10);
        locat = new GeoLocation(1010, (byte) 3, (byte) 21);
        result.putGeoLocation(locat, 10);

        client.setGeoLocations(result);
        client.calculatePoiCommVector();
        assertEquals(expected, client.getRepresGeoLocations());

    }

    @Test
    public void testComplete() {
        locat = new GeoLocation(10, (byte) 1, (byte) 12);
        result.putGeoLocation(locat, 30);
        locat = new GeoLocation(10, (byte) 3, (byte) 12);
        result.putGeoLocation(locat, 20);
        locat = new GeoLocation(10, (byte) 2, (byte) 10);
        result.putGeoLocation(locat, 20);
        locat = new GeoLocation(10, (byte) 7, (byte) 23);
        result.putGeoLocation(locat, 30);
        locat = new GeoLocation(10, (byte) 4, (byte) 10);
        result.putGeoLocation(locat, 20);
        locat = new GeoLocation(10, (byte) 6, (byte) 22);
        result.putGeoLocation(locat, 50);
        locat = new GeoLocation(11, (byte) 7, (byte) 9);
        result.putGeoLocation(locat, 10);
        locat = new GeoLocation(10, (byte) 5, (byte) 8);
        result.putGeoLocation(locat, 20);
        // Output
        expected.add((long) 10);

        client.setGeoLocations(result);
        client.calculatePoiCommVector();
        assertEquals(expected, client.getRepresGeoLocations());
    }

}
