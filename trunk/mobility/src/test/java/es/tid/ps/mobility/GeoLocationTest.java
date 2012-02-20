package es.tid.ps.mobility;

import static org.junit.Assert.assertEquals;

import org.junit.After;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import es.tid.ps.mobility.data.GeoLocation;

public class GeoLocationTest {

	GeoLocation geoLocation;
	GeoLocation geoLocationExpected;
	GeoLocation geoLocationNotIni;

	@Before
	public void setUp() throws Exception {
		geoLocation = new GeoLocation((long) 1, (byte) 2, (byte) 3);
		geoLocationNotIni = new GeoLocation((long) -1, (byte) -1, (byte) -1);
	}

	@After
	public void tearDown() throws Exception {
	}

	@Test
	public void testGetIdPlace() {
		assertEquals(1, geoLocation.getIdPlace());
	}

	@Test
	public void testSetIdPlace() {
		geoLocationExpected = new GeoLocation((long) 44, (byte) 2, (byte) 3);

		geoLocation.setIdPlace(44);

		assertEquals(geoLocationExpected, geoLocation);
	}

	@Test
	public void testGetWeekDay() {
		assertEquals((byte) 2, (byte) geoLocation.getWeekDay());
	}

	@Test
	public void testSetWeekDay() {
		geoLocationExpected = new GeoLocation((long) 1, (byte) 3, (byte) 3);

		geoLocation.setWeekDay((byte) 3);

		assertEquals(geoLocationExpected, geoLocation);
	}

	@Test
	public void testGetHour() {
		assertEquals((byte) 3, (byte) geoLocation.getHour());
	}

	@Test
	public void testSetHour() {
		geoLocationExpected = new GeoLocation((long) 1, (byte) 2, (byte) 24);

		geoLocation.setHour((byte) 24);

		assertEquals(geoLocationExpected, geoLocation);
	}

	// cleanGeoLocation
	@Test
	public void testCleanGeoLocation() {
		geoLocation.cleanGeoLocation();

		assertEquals(geoLocationNotIni, geoLocation);
	}

	// equals
	@Test
	public void testEqualsThis() {
		GeoLocation geoLocationAux = geoLocation;
		boolean actual = geoLocation.equals((GeoLocation) geoLocationAux);

		assertEquals(new Boolean(true), actual);
	}

	@Test
	public void testEqualsNull() {
		boolean actual = geoLocation.equals(null);

		assertEquals(new Boolean(false), actual);
	}

	@Test
	public void testEqualsNotCast() {
		boolean actual = geoLocation.equals(new Integer(1));

		assertEquals(new Boolean(false), actual);
	}

	@Test
	public void testEqualsEqual() {
		boolean actual = geoLocation.equals((GeoLocation) new GeoLocation(
				(long) 1, (byte) 2, (byte) 3));

		assertEquals(new Boolean(true), actual);
	}

	@Test
	public void testEqualsNotEqual() {
		boolean actual = geoLocation.equals((GeoLocation) geoLocationNotIni);

		assertEquals(new Boolean(false), actual);
	}

	// CompareTo
	@Test
	public void testCompareToWithNull() {
		try {
			geoLocation.compareTo(null);
			Assert.fail("Exception_should_be_thrown");
		} catch (NullPointerException e) {
			//TODO
		}
	}

	@Test
	public void testCompareToEquals() {
		geoLocationExpected = geoLocation;
		int actual = geoLocation.compareTo(geoLocation);
		assertEquals(0, actual);
	}

	@Test
	public void testCompareToMajor() {
		geoLocationExpected = new GeoLocation((long) 2, (byte) 2, (byte) 3);
		int actual = geoLocation.compareTo(geoLocationExpected);
		assertEquals(-1, actual);
	}

	@Test
	public void testCompareToMajor2() {
		geoLocationExpected = new GeoLocation((long) 1, (byte) 3, (byte) 3);
		int actual = geoLocation.compareTo(geoLocationExpected);
		assertEquals(-1, actual);
	}

	@Test
	public void testCompareToMajor3() {
		geoLocationExpected = new GeoLocation((long) 1, (byte) 2, (byte) 4);
		int actual = geoLocation.compareTo(geoLocationExpected);
		assertEquals(-1, actual);
	}

	@Test
	public void testCompareToMinor() {
		geoLocationExpected = new GeoLocation((long) 0, (byte) 2, (byte) 3);
		int actual = geoLocation.compareTo(geoLocationExpected);
		assertEquals(1, actual);
	}

	@Test
	public void testCompareToMinor2() {
		geoLocationExpected = new GeoLocation((long) 1, (byte) 1, (byte) 3);
		int actual = geoLocation.compareTo(geoLocationExpected);
		assertEquals(1, actual);
	}

	@Test
	public void testCompareToMinor3() {
		geoLocationExpected = new GeoLocation((long) 1, (byte) 2, (byte) 1);
		int actual = geoLocation.compareTo(geoLocationExpected);
		assertEquals(2, actual);
	}

}
