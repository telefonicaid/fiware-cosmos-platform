package es.tid.mapreduce.mobility2.data;

import java.util.List;

/**
 * This class storage the area of influence. A geographical area wherein a
 * commander is directly capable of influencing operations by maneuver or fire
 * support systems normally under the commander's command or control.
 * 
 * The area of influence is defined by the longitude, latitude and the radio.
 * 
 * @author rgc
 */
public class AreaOfInfluence {
    private String identifier;
    private Double longitude;
    private Double latitude;
    private Double radio;

    /**
     * Constructor
     */
    public AreaOfInfluence() {
        super();
    }

    /**
     * Constructor
     * 
     * @param identifier
     *            the identifier of the area
     * @param longitude
     *            the longitude position of the area
     * @param latitude
     *            the latitude position of the area
     * @param radio
     *            the radio of the area
     */
    public AreaOfInfluence(String identifier, Double longitude,
            Double latitude, Double radio) {
        super();
        this.longitude = longitude;
        this.latitude = latitude;
        this.radio = radio;
        this.identifier = identifier;
    }

    /**
     * @return the identifier
     */
    public String getIdentifier() {
        return identifier;
    }

    /**
     * @return the longitude
     */
    public Double getLongitude() {
        return longitude;
    }

    /**
     * @return the latitude
     */
    public Double getLatitude() {
        return latitude;
    }

    /**
     * @return the radio
     */
    public Double getRadio() {
        return radio;
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#hashCode()
     */
    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result
                + ((identifier == null) ? 0 : identifier.hashCode());
        result = prime * result
                + ((latitude == null) ? 0 : latitude.hashCode());
        result = prime * result
                + ((longitude == null) ? 0 : longitude.hashCode());
        result = prime * result + ((radio == null) ? 0 : radio.hashCode());
        return result;
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#equals(java.lang.Object)
     */
    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null)
            return false;
        if (getClass() != obj.getClass())
            return false;
        AreaOfInfluence other = (AreaOfInfluence) obj;
        if (identifier == null) {
            if (other.identifier != null)
                return false;
        } else if (!identifier.equals(other.identifier))
            return false;
        if (latitude == null) {
            if (other.latitude != null)
                return false;
        } else if (!latitude.equals(other.latitude))
            return false;
        if (longitude == null) {
            if (other.longitude != null)
                return false;
        } else if (!longitude.equals(other.longitude))
            return false;
        if (radio == null) {
            if (other.radio != null)
                return false;
        } else if (!radio.equals(other.radio))
            return false;
        return true;
    }

    /**
     * Static method that generate the area of influence of some bts
     * 
     * @param indentifer
     *            the identifier of the area of influence
     * @param elements
     *            a list with the data of the bts
     * @return a pojo with the area of influence
     */
    public static AreaOfInfluence calculateCenterOfMass(String indentifer,
            List<CellData> elements) {
        Double longitude = new Double(0);
        Double latitude = new Double(0);
        Double radio = new Double(0);
        int numElem = elements.size();
        for (CellData cd : elements) {
            longitude += cd.getLongitude();
            latitude += cd.getLatitude();
        }
        longitude /= numElem;
        latitude /= numElem;

        for (CellData cd : elements) {
            radio += Math.sqrt(Math.pow(2, (cd.getLongitude() - longitude))
                    + (Math.pow(2, (cd.getLongitude() - longitude))));
        }
        radio /= numElem;
        return new AreaOfInfluence(indentifer, longitude, latitude, radio);
    }
}
