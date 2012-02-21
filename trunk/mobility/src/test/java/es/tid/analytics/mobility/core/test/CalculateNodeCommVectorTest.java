package es.tid.analytics.mobility.core.test;

import es.tid.analytics.mobility.core.data.Client;
import es.tid.analytics.mobility.core.data.GeoLocation;
import es.tid.analytics.mobility.core.data.GeoLocationContainer;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import java.util.ArrayList;
import java.util.Iterator;

public class CalculateNodeCommVectorTest {

	private Client client;

	private ArrayList<Double> bound(final ArrayList<Double> vector) {
		final ArrayList<Double> vectorOut = new ArrayList<Double>();

		final Iterator<Double> iter = vector.iterator();

		while (iter.hasNext()) {
			vectorOut.add((int) (iter.next() * 1000000) / 1000000.0);
		}

		return vectorOut;
	}

	@Before
	public void setUp() {
		client = new Client();

		final GeoLocationContainer locations = new GeoLocationContainer();
		// locations.setGeolocations(new TreeMap<GeoLocation,Integer>());

		client.setGeoLocations(locations);
	}

	@Test
	public void testEmpty() {
		final ArrayList<Double> result;
		final ArrayList<Double> expected = new ArrayList<Double>();

		client.calculateNodeCommVector();
		result = (ArrayList<Double>) client.getNodeCommVector();

		assertEquals(expected, result);
	}

	@Test
	public void test() {
		ArrayList<Double> result;
		final ArrayList<Double> expected = new ArrayList<Double>();

		for (int i = 0; i < 96; i++) {
			expected.add(0.0);
		}

		GeoLocation loc = new GeoLocation();

		loc.setIdPlace(1);
		loc.setWeekday((byte) 0);
		loc.setHour((byte) 0);
		client.getGeoLocations().putGeoLocation(loc, 1);

		loc = new GeoLocation();
		loc.setIdPlace(2);
		loc.setWeekday((byte) 1);
		loc.setHour((byte) 12);
		client.getGeoLocations().putGeoLocation(loc, 1);

		loc = new GeoLocation();
		loc.setIdPlace(2);
		loc.setWeekday((byte) 2);
		loc.setHour((byte) 23);
		client.getGeoLocations().putGeoLocation(loc, 1);

		loc = new GeoLocation();
		loc.setIdPlace(5);
		loc.setWeekday((byte) 2);
		loc.setHour((byte) 23);
		client.getGeoLocations().putGeoLocation(loc, 4);

		loc = new GeoLocation();
		loc.setIdPlace(2);
		loc.setWeekday((byte) 3);
		loc.setHour((byte) 0);
		client.getGeoLocations().putGeoLocation(loc, 1);

		client.calculateNodeCommVector();
		result = (ArrayList<Double>) client.getNodeCommVector();
		result = this.bound(result);

		expected.set(0, 0.183098);
		expected.set(23, 0.457746);
		expected.set(84, 0.359154);

		assertEquals(expected, result);
	}

}
