package es.tid.bdp.mobility.parsing;

import org.junit.Assert;
import static org.junit.Assert.assertEquals;
import org.junit.Test;

import es.tid.bdp.mobility.data.GeoLocation;
import es.tid.bdp.mobility.data.GeoLocationContainer;

public class GeoLocationContainerTest {
    @Test
    public void testGeoLocationContainer() {
        GeoLocation geoLocation = new GeoLocation(666, (byte) 34, (byte) 5);
        GeoLocationContainer geoLocationsActual = new GeoLocationContainer();
        GeoLocationContainer geoLocationsExpected = new GeoLocationContainer();

        geoLocationsExpected.putGeoLocation(geoLocation, Integer.valueOf(1));
        geoLocationsActual.incrementGeoLocation(geoLocation);

        assertEquals(geoLocationsExpected, geoLocationsActual);
    }

    @Test
    public void testGeoLocationContainerNull() {
        GeoLocation geoLocation = null;
        GeoLocationContainer geoLocationsActual = new GeoLocationContainer();
        GeoLocationContainer geoLocationsExpected = new GeoLocationContainer();

        try {

            geoLocationsActual.incrementGeoLocation(geoLocation);
            Assert.fail("Se debe lanzar una excepcion");
        } catch (Exception unused) {
            geoLocation = new GeoLocation(666, (byte) 34, (byte) 5);
            geoLocationsActual.incrementGeoLocation(geoLocation);
            geoLocationsExpected.putGeoLocation(geoLocation, Integer.valueOf(1));
            assertEquals(geoLocationsExpected,
                    geoLocationsActual);

        }

    }
}
