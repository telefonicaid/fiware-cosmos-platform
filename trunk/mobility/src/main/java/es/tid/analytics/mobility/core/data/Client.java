package es.tid.analytics.mobility.core.data;

import org.apache.log4j.Logger;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

public class Client {
	private static final Logger LOG = Logger.getLogger(Client.class);

	private static final int TAMNOINFOGEOLOCATION = 168;
	private static final int TAMNODECOMMVECTOR = 96;
	private static final int TAMPOICOMMVECTOR = 96;

	private long userId;
	private GeoLocationContainer geoLocations;
	private List<Integer> noInfoGeolocations;
	private List<Long> represGeoLocations;
	private List<Double> nodeCommVector;
	private List<Poi> pois;

	public Client() {
		this.geoLocations = new GeoLocationContainer();
		this.noInfoGeolocations = new ArrayList<Integer>();
		this.represGeoLocations = new ArrayList<Long>();
		this.nodeCommVector = new ArrayList<Double>();
		this.pois = new ArrayList<Poi>();
	}

	public void calculateGeoLocations(final Iterator<GLEvent> iteratorGLEvent) {

		GLEvent glEvent;
		int position;
		int value;

		while (iteratorGLEvent.hasNext()) {
			glEvent = iteratorGLEvent.next();
			final GeoLocation geoLocation = new GeoLocation();

			if (glEvent.getPlaceId() != 0) {
				geoLocation.setIdPlace(glEvent.getPlaceId());
				geoLocation.setWeekday(glEvent.getWeekDay());
				geoLocation.setHour(glEvent.getHour());
				geoLocations.incrementGeoLocation(geoLocation);
			} else {
				position = Utils.getPosListNoInfoGeolocations(
						glEvent.getWeekDay(), glEvent.getHour());
				if (position != -1) {
					if (noInfoGeolocations.isEmpty()) {
						initNoInfoGeolocations(TAMNOINFOGEOLOCATION);
					}
					value = noInfoGeolocations.get(position);
					value++;
					noInfoGeolocations.set(position, value);

				}
			}
		}
		this.calculateRepresGeoLocations();
	}

	private void calculateRepresGeoLocations() {
		int totalNumEvents, numEvents;
		double percLocation;
		Iterator<Entry<GeoLocation, Integer>> iter = this.geoLocations
				.getIterator();

		// Calculate total number of events
		totalNumEvents = 0;
		while (iter.hasNext()) {
			final Map.Entry<GeoLocation, Integer> me = iter.next();
			totalNumEvents += me.getValue();
		}
		// Filter by total number of events --> No representative locations
		if (totalNumEvents < 200 || totalNumEvents > 5000) {
			return;
		}
		// Calculate percentage of location.
		long curLoc, nextLoc;
		curLoc = -1;
		numEvents = 0;

		iter = this.geoLocations.getIterator();
		while (iter.hasNext()) {
			Map.Entry<GeoLocation, Integer> me = iter.next();
			curLoc = me.getKey().getIdPlace();
			numEvents += me.getValue();
			while (iter.hasNext()) {
				me = iter.next();
				nextLoc = me.getKey().getIdPlace();
				if (curLoc == nextLoc) {
					numEvents += me.getValue();
				} else {
					percLocation = (double) numEvents / (double) totalNumEvents;
					if (numEvents >= 14 && percLocation >= 0.05) {
						this.represGeoLocations.add(curLoc);
					}
					numEvents = me.getValue();
					curLoc = nextLoc;
				}
			}
		}
		percLocation = (double) numEvents / (double) totalNumEvents;
		if (numEvents >= 14 && percLocation >= 0.05) {
			this.represGeoLocations.add(curLoc);
		}
	}

	public void calculateNodeCommVector() {
		nodeCommVector = new ArrayList<Double>();
		int comunicationsPos;
		final Iterator<GeoLocation> iterator = this.geoLocations
				.getKeysIterator();

		if (iterator.hasNext()) {
			for (int i = 0; i < TAMNODECOMMVECTOR; i++) {
				nodeCommVector.add(0.0);
			}
		}

		while (iterator.hasNext()) {
			final GeoLocation geo = iterator.next();

			comunicationsPos = Utils.getPosVectorComunications(
					geo.getWeekday(), geo.getHour());
			// LOG.debug("geo.getWeekDay()=" + geo.getWeekDay() +
			// ", geo.getHour()=" + geo.getHour());
			this.nodeCommVector.set(comunicationsPos,
					this.nodeCommVector.get(comunicationsPos)
							+ this.geoLocations.getGeolocation(geo));
		}

		if (!noInfoGeolocations.isEmpty()) {
			// Add info of no InfoGeoLocation
			for (int position = 0; position < TAMNOINFOGEOLOCATION; position++) {

				final int value = this.noInfoGeolocations.get(position);
				if (value != 0) {
					comunicationsPos = Utils
							.getPosAddNoInfoGeolocations(position);
					this.nodeCommVector.set(comunicationsPos,
							this.nodeCommVector.get(comunicationsPos) + value);
				}
			}
		}

		nodeCommVector = Utils
				.normalizationOfVector((ArrayList<Double>) nodeCommVector);
	}

	public void calculatePoiCommVector() {
		long locId;
		final ArrayList<Double> eventsArray = new ArrayList<Double>();
		final GeoLocation locat = new GeoLocation();
		int numEvents;
		int pos;

		// Calculate representative geolocations
		if (getRepresGeoLocations().isEmpty()) {
			this.calculateRepresGeoLocations();
		}
		// Calculate events vector for representative locations
		final Iterator<Long> iterArr = this.represGeoLocations.iterator();

		while (iterArr.hasNext()) {
			locId = iterArr.next();
			// Create Poi
			final Poi poi = new Poi();
			poi.setGeoLocationId(locId);

			// Initialize events array
			eventsArray.clear();
			for (int i = 0; i < TAMPOICOMMVECTOR; i++) {
				eventsArray.add(0.000000);
			}

			// Calculate events vector
			for (byte weekday = Calendar.SUNDAY; weekday <= Calendar.SATURDAY; weekday++) {
				for (byte hour = 0; hour < 24; hour++) {
					locat.cleanGeoLocation();
					locat.setIdPlace(locId);
					locat.setWeekday(weekday);
					locat.setHour(hour);

					if (this.geoLocations.containsGeolocation(locat)) {
						numEvents = this.geoLocations.getGeolocation(locat);

						// Add num of events to events vector
						pos = Utils.getPosVectorComunications(weekday, hour);
						eventsArray.set(pos, eventsArray.get(pos) + numEvents);
					}
				}
			}

			poi.setCommVector(Utils.normalizationOfVector(eventsArray));
			this.pois.add(poi);
		}
	}

	public long getUserId() {
		return userId;
	}

	public void setUserId(final long idNode) {
		this.userId = idNode;
	}

	public GeoLocationContainer getGeoLocations() {
		return geoLocations;
	}

	public void setGeoLocations(final GeoLocationContainer geoLocations) {
		this.geoLocations = geoLocations;
	}

	public List<Integer> getNoInfoGeolocations() {
		return noInfoGeolocations;
	}

	public void setNoInfoGeolocations(final List<Integer> noInfoGeolocations) {
		this.noInfoGeolocations = noInfoGeolocations;
	}

	public List<Long> getRepresGeoLocations() {
		return represGeoLocations;
	}

	public void setRepresGeoLocations(final List<Long> represGeoLocations) {
		this.represGeoLocations = represGeoLocations;
	}

	public List<Double> getNodeCommVector() {
		return nodeCommVector;
	}

	public void setNodeCommVector(final List<Double> nodeCommVector) {
		this.nodeCommVector = nodeCommVector;
	}

	public List<Poi> getPois() {
		return pois;
	}

	public void setPois(final List<Poi> pois) {
		this.pois = pois;
	}

	private void initNoInfoGeolocations(final int size) {

		for (int position = 0; position < size; position++) {
			noInfoGeolocations.add(0);
		}
	}

	private int nuemElementsNoInfoGeolocations() {
		int result = 0, value;

		if (noInfoGeolocations.size() != 0) {
			for (int position = 0; position < TAMNOINFOGEOLOCATION; position++) {
				value = noInfoGeolocations.get(position);
				if (value != 0) {
					result = result + value;
				}
			}
		}

		return result;
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result
				+ ((geoLocations == null) ? 0 : geoLocations.hashCode());
		result = prime * result + (int) (userId ^ (userId >>> 32));
		result = prime
				* result
				+ ((noInfoGeolocations == null) ? 0 : noInfoGeolocations
						.hashCode());
		result = prime * result
				+ ((nodeCommVector == null) ? 0 : nodeCommVector.hashCode());
		result = prime * result + ((pois == null) ? 0 : pois.hashCode());
		result = prime
				* result
				+ ((represGeoLocations == null) ? 0 : represGeoLocations
						.hashCode());
		return result;
	}

	@Override
	public boolean equals(final Object obj) {
		if (this == obj) {
			return true;
		}
		if (obj == null) {
			return false;
		}
		if (getClass() != obj.getClass()) {
			return false;
		}
		final Client other = (Client) obj;
		if (geoLocations == null) {
			if (other.geoLocations != null) {
				return false;
			}
		} else if (!geoLocations.equals(other.geoLocations)) {
			return false;
		}
		if (userId != other.userId) {
			return false;
		}
		if (noInfoGeolocations == null) {
			if (other.noInfoGeolocations != null) {
				return false;
			}
		} else if (!noInfoGeolocations.equals(other.noInfoGeolocations)) {
			return false;
		}
		if (nodeCommVector == null) {
			if (other.nodeCommVector != null) {
				return false;
			}
		} else if (!nodeCommVector.equals(other.nodeCommVector)) {
			return false;
		}
		if (pois == null) {
			if (other.pois != null) {
				return false;
			}
		} else if (!pois.equals(other.pois)) {
			return false;
		}
		if (represGeoLocations == null) {
			if (other.represGeoLocations != null) {
				return false;
			}
		} else if (!represGeoLocations.equals(other.represGeoLocations)) {
			return false;
		}
		return true;
	}

	@Override
	public String toString() {
		return "Client [idNode=" + userId + ", geoLocations=" + geoLocations
				+ ", noInfoGeolocations=" + noInfoGeolocations
				+ ", numNoInfoGeolocations=" + nuemElementsNoInfoGeolocations()
				+ ", represGeoLocations=" + represGeoLocations
				+ ", nodeCommVector=" + nodeCommVector + ", pois=" + pois + "]";
	}
}
