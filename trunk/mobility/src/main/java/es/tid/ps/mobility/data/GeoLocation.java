package es.tid.ps.mobility.data;

public class GeoLocation implements Comparable<GeoLocation> {

	private long idPlace;
	private byte weekDay;
	private byte hour;

	public GeoLocation() {
		idPlace = -1;
		weekDay = -1;
		hour = -1;
	}

	public GeoLocation(final long idPl, final byte w, final byte h) {
		this.idPlace = idPl;
		this.weekDay = w;
		this.hour = h;
	}

	public final long getIdPlace() {
		return idPlace;
	}

	public final void setIdPlace(final long idPl) {
		this.idPlace = idPl;
	}

	public final byte getWeekDay() {
		return weekDay;
	}

	public final void setWeekDay(final byte w) {
		this.weekDay = w;
	}

	public byte getHour() {
		return this.hour;
	}

	public final void setHour(final byte h) {
		this.hour = h;
	}

	public final void cleanGeoLocation() {
		this.idPlace = -1;
		this.weekDay = -1;
		this.hour = -1;

	}

	@Override
	public final int compareTo(final GeoLocation geolocationCompare) {
		if (geolocationCompare == null) {
			throw new NullPointerException();
		}
		int comp;
		comp = (int) (this.idPlace - geolocationCompare.idPlace);
		if (comp == 0) {
			comp = this.weekDay - geolocationCompare.weekDay;
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
		result = prime * result + (int) (idPlace ^ (idPlace >>> 32));
		result = prime * result + weekDay;
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
		if (idPlace != other.idPlace) {
			return false;
		}
		if (weekDay != other.weekDay) {
			return false;
		}
		return true;
	}

	@Override
	public String toString() {
		return "GeoLocation [idPlace=" + idPlace + ", weekDay=" + weekDay
				+ ", hour=" + hour + "]";
	}

}
