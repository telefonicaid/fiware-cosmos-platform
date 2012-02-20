package es.tid.ps.mobility;

import es.tid.ps.mobility.data.Client;
import es.tid.ps.mobility.data.GeoLocation;
import es.tid.ps.mobility.data.GeoLocationContainer;
import es.tid.ps.mobility.data.Poi;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import java.util.ArrayList;

public class CalculatePoiCommVectorTest {
    private Client client;
    private ArrayList<Poi> result, expected;
    private GeoLocation locat;
    private GeoLocationContainer container;

    @Before
    public void setUp() {
        client = new Client();
        result = new ArrayList<Poi>();
        expected = new ArrayList<Poi>();
        container = new GeoLocationContainer();
    }

    @Test
    public void testNodeFiltered() {
        // Input
        locat = new GeoLocation(1010, (byte) 6, (byte) 21);
        container.putGeoLocation(locat, 10);
        locat = new GeoLocation(1010, (byte) 3, (byte) 21);
        container.putGeoLocation(locat, 10);
        // Run
        client.setGeoLocations(container);
        client.calculatePoiCommVector();
        result = (ArrayList<Poi>) client.getPois();
//        assertEquals(expected,  client.getRepresGeoLocations());
        assertEquals(expected, result);
    }

    @Test
    public void TestOnePoi() {
        //Input
        locat = new GeoLocation(10, (byte) 1, (byte) 12);
        container.putGeoLocation(locat, 30);
        locat = new GeoLocation(10, (byte) 3, (byte) 12);
        container.putGeoLocation(locat, 20);
        locat = new GeoLocation(10, (byte) 2, (byte) 10);
        container.putGeoLocation(locat, 20);
        locat = new GeoLocation(10, (byte) 7, (byte) 23);
        container.putGeoLocation(locat, 30);
        locat = new GeoLocation(10, (byte) 4, (byte) 10);
        container.putGeoLocation(locat, 20);
        locat = new GeoLocation(10, (byte) 6, (byte) 22);
        container.putGeoLocation(locat, 50);
        locat = new GeoLocation(11, (byte) 7, (byte) 9);
        container.putGeoLocation(locat, 10);
        locat = new GeoLocation(10, (byte) 5, (byte) 8);
        container.putGeoLocation(locat, 20);
        client.setGeoLocations(container);
        //Output
        final Poi poi = new Poi();
        final ArrayList<Double> comms = new ArrayList<Double>();
        for (int pos = 0; pos < 96; pos++) {
            switch (pos) {
                case 10:
                    comms.add(0.083277);
                    break;
                case 12:
                    comms.add(0.104096);
                    break;
                case 32:
                    comms.add(0.162525);
                    break;
                case 70:
                    comms.add(0.406312);
                    break;
                case 95:
                    comms.add(0.243787);
                    break;
                default:
                    comms.add(0.0);
                    break;
            }
        }
        poi.setPoiId(10);
        poi.setCommVector(comms);
        client.getPois().add(poi);
        //Run
        client.calculatePoiCommVector();
        assertEquals(expected, result);
    }

    @Test
    public void testComplete() {
        // Input
        locat = new GeoLocation(2150, (byte) 6, (byte) 22);
        container.putGeoLocation(locat, 160);
        locat = new GeoLocation(2151, (byte) 2, (byte) 13);
        container.putGeoLocation(locat, 170);
        locat = new GeoLocation(2152, (byte) 2, (byte) 10);
        container.putGeoLocation(locat, 100);
        client.setGeoLocations(container);
        //Output
        final Poi poi = new Poi();
        final ArrayList<Double> comms = new ArrayList<Double>();
        poi.setPoiId(2150);
        for (int i = 0; i < 96; i++) {
            if (i == 70) {
                comms.add(1.0);
            } else {
                comms.add(0.0);
            }
        }
        poi.setCommVector(comms);
        client.getPois().add(poi);
        poi.setPoiId(2151);
        for (int i = 0; i < 96; i++) {
            if (i == 13) {
                comms.add(1.0);
            } else {
                comms.add(0.0);
            }
        }
        poi.setCommVector(comms);
        client.getPois().add(poi);
        poi.setPoiId(2152);
        for (int i = 0; i < 96; i++) {
            if (i == 10) {
                comms.add(1.0);
            } else {
                comms.add(0.0);
            }
        }
        poi.setCommVector(comms);
        client.getPois().add(poi);
        //Run
        client.calculatePoiCommVector();
        assertEquals(expected, result);
    }
}