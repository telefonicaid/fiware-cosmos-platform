package es.tid.analytics.mobility.core.test;

import static org.junit.Assert.assertEquals;

import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import es.tid.analytics.mobility.core.data.Client;
import es.tid.analytics.mobility.core.data.GLEvent;
import es.tid.analytics.mobility.core.data.GeoLocation;
import es.tid.analytics.mobility.core.data.GeoLocationContainer;
import es.tid.analytics.mobility.core.data.Utils;

public class ClientTest {

	@Before
	public void setUp() throws Exception {
	}

	@After
	public void tearDown() throws Exception {
	}

	@SuppressWarnings("deprecation")
	@Test
	public void testCalculateGeoLocationsNoInfoGeolocations() {
		Client client = new Client();
		List<GLEvent> listGLEvent = new ArrayList<GLEvent>();
		List<Integer> noInfoGeolocationsExpected = new ArrayList<Integer>();
		List<Integer> noInfoGeolocationsActual = null;
		GLEvent glEvent = new GLEvent(0, 0,
				new Date((2012 - 1900), 1, 4, 23, 5));

		for (int position = 0; position < 168; position++) {
			noInfoGeolocationsExpected.add(position, new Integer(0));
		}

		noInfoGeolocationsExpected.set(167, new Integer(1));

		listGLEvent.add(glEvent);
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

		GLEvent glEvent = new GLEvent(0, 333, new Date((2012 - 1900), 1, 4, 23,
				5));
		listGLEvent.add(glEvent);

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
