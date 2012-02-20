package es.tid.ps.mobility.data;

import java.util.ArrayList;

public class Poi {

    private int poiId;
    private long geoLocationId;
    private int labelLvl0;
    private int labelLvl1;
    private boolean reliable;
    private ArrayList<Double> commVector;

    public Poi() {
        this.commVector = new ArrayList<Double>();
    }

    public int getPoiId() {
        return poiId;
    }

    public void setPoiId(final int poiId) {
        this.poiId = poiId;
    }

    public long getGeoLocationId() {
        return geoLocationId;
    }

    public void setGeoLocationId(final long geoLocationId) {
        this.geoLocationId = geoLocationId;
    }

    public int getLabelLvl0() {
        return labelLvl0;
    }

    public void setLabelLvl0(final int labelLvl0) {
        this.labelLvl0 = labelLvl0;
    }

    public int getLabelLvl1() {
        return labelLvl1;
    }

    public void setLabelLvl1(final int labelLvl1) {
        this.labelLvl1 = labelLvl1;
    }

    public boolean isReliable() {
        return reliable;
    }

    public void setReliable(final boolean reliable) {
        this.reliable = reliable;
    }

    public ArrayList<Double> getCommVector() {
        return commVector;
    }

    public void setCommVector(final ArrayList<Double> commVector) {
        this.commVector = commVector;
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((commVector == null) ? 0 : commVector.hashCode());
        result = prime * result + (int) (geoLocationId ^ (geoLocationId >>> 32));
        result = prime * result + labelLvl0;
        result = prime * result + labelLvl1;
        result = prime * result + poiId;
        result = prime * result + (reliable ? 1231 : 1237);
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
        final Poi other = (Poi) obj;
        if (commVector == null) {
            if (other.commVector != null) {
                return false;
            }
        } else if (!commVector.equals(other.commVector)) {
            return false;
        }
        if (geoLocationId != other.geoLocationId) {
            return false;
        }
        if (labelLvl0 != other.labelLvl0) {
            return false;
        }
        if (labelLvl1 != other.labelLvl1) {
            return false;
        }
        if (poiId != other.poiId) {
            return false;
        }
        if (reliable != other.reliable) {
            return false;
        }
        return true;
    }

    @Override
    public String toString() {
        return "Poi [poiId=" + poiId + ", geoLocationId=" + geoLocationId + ", labelLvl0=" + labelLvl0 + ", labelLvl1=" + labelLvl1 + ", reliable=" +
                reliable + ", commVector=" + commVector + "]";
    }
}
