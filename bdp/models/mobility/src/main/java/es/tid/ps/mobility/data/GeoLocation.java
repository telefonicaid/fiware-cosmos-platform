package es.tid.ps.mobility.data;

public class GeoLocation implements Comparable<GeoLocation> {

	private long placeId;
	private byte weekday;
	private byte hour;

	public GeoLocation() {
		placeId = -1;
		weekday = -1;
		hour = -1;
	}

	public GeoLocation(final long idPl, final byte w, final byte h) {
		this.placeId = idPl;
		this.weekday = w;
		this.hour = h;
	}

	public final long getIdPlace() {
		return placeId;
	}

	public final void setIdPlace(final long idPl) {
		this.placeId = idPl;
	}

	public final byte getWeekday() {
		return weekday;
	}

	public final void setWeekday(final byte w) {
		this.weekday = w;
	}

	public byte getHour() {
		return this.hour;
	}

	public final void setHour(final byte h) {
		this.hour = h;
	}

	public final void cleanGeoLocation() {
		this.placeId = -1;
		this.weekday = -1;
		this.hour = -1;

	}

	@Override
	public final int compareTo(final GeoLocation geolocationCompare) {
		if (geolocationCompare == null) {
			throw new NullPointerException();
		}
		int comp;
		comp = (int) (this.placeId - geolocationCompare.placeId);
		if (comp == 0) {
			comp = this.weekday - geolocationCompare.weekday;
			if (comp == 0) {
				comp = this.hour - geolocationCompare.hour;
			}
		}
		return comp;
	}

	@Override
	public final int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + hour;
		result = prime * result + (int) (placeId ^ (placeId >>> 32));
		result = prime * result + weekday;
		return result;
	}

	@Override
	public final boolean equals(final Object obj) {
		if (this == obj) {
			return true;
		}
		if (obj == null) {
			return false;
		}
		if (getClass() != obj.getClass()) {
			return false;
		}
		final GeoLocation other = (GeoLocation) obj;
		if (hour != other.hour) {
			return false;
		}
		if (placeId != other.placeId) {
			return false;
		}
		if (weekday != other.weekday) {
			return false;
		}
		return true;
	}

	@Override
	public String toString() {
		return "GeoLocation [idPlace=" + placeId + ", weekDay=" + weekday
				+ ", hour=" + hour + "]";
	}

}
