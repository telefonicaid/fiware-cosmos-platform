package es.tid.ps.mobility.data;

import java.util.Iterator;
import java.util.Map;
import java.util.TreeMap;

public class GeoLocationContainer {

    private Map<GeoLocation, Integer> geoLocations;

    public GeoLocationContainer() {
        this.geoLocations = new TreeMap<GeoLocation, Integer>();
    }


    /**
     * Add a geolocation. If it exists, increment the number of comunications in 1
     * If not exist create an update with 1
     *
     * @param geoLocation - to increment
     */
    public final void incrementGeoLocation(final GeoLocation geoLocation) {
        if (this.geoLocations.containsKey(geoLocation)) {
            final int counter = this.geoLocations.get(geoLocation);
            geoLocations.put(geoLocation, counter + 1);
        } else {
        	geoLocations.put(geoLocation, 1);
        }
    }


    /**
     * Add a geolocation. If it exists, increment the number of comunications with value field.
     * If not exist create an update with the value
     *
     * @param geoLocation - to increment
     * @param value       - number of comunications to increment
     */
    public final void incrementGeoLocation(final GeoLocation geoLocation, final int value) {
        if (this.geoLocations.containsKey(geoLocation)) {
            final int counter = this.geoLocations.get(geoLocation);
            geoLocations.put(geoLocation, counter + value);
        } else {
        	geoLocations.put(geoLocation, value);
        }
    }

    /**
     * Add a geolocation. If it exists, overrides the geoLocations
     *
     * @param geoLocation - to add or modify
     * @param value       - new number of comunications
     */
    public final void putGeoLocation(final GeoLocation geoLocation, final int value) {
        this.geoLocations.put(geoLocation, value);
    }

    /**
     * Accumulates the values of geoLocations 
     *  
     * @return values accumulates of geoLocations
     */
    public final int getNumGeoLoc() {
        final Iterator<Integer> iterator = this.geoLocations.values().iterator();
        int cont = 0;
        while (iterator.hasNext()) {
            cont = cont + iterator.next();
        }
        return cont;
    }

    /**
     * Obtain the number of comunications for a geoLocation
     *
     * @param geoLocation - to obtain the number of comunications
     * @return int - number of comunications
     */
    public final int getGeolocation(final GeoLocation geoLocation) {
        if (this.geoLocations.containsKey(geoLocation)) {
            return this.geoLocations.get(geoLocation);
        } else {
            return 0;
        }
    }

    /**
     * check if a geoLocation exists.
     *
     * @param key - to check
     * @return boolean - true if exists
     */
    public final boolean containsGeolocation(final GeoLocation key) {
        return this.geoLocations.containsKey(key);
    }

    /**
     * Obtain an Iterator for the entry set
     *
     * @return Iterator - an Iterator for the Entry Set
     */
    public final Iterator<Map.Entry<GeoLocation, Integer>> getIterator() {
        return this.geoLocations.entrySet().iterator();
    }

    /**
     * Obtain an Iterator for the Keys
     *
     * @return Iterator - an Iterator for the Key Set
     */
    public final Iterator<GeoLocation> getKeysIterator() {
        return this.geoLocations.keySet().iterator();
    }

    @Override
    public final int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((geoLocations == null) ? 0 : geoLocations.hashCode());
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
        final GeoLocationContainer other = (GeoLocationContainer) obj;
        if (geoLocations == null) {
            if (other.geoLocations != null) {
                return false;
            }
        } else if (!geoLocations.equals(other.geoLocations)) {
            return false;
        }
        return true;
    }

    @Override
    public final String toString() {
        return "GeoLocationContainer [geolocations=" + geoLocations + ", numgeolocationsdiferentes=" + geoLocations.size() +
                ", numGeoLocationsTotales=" + getNumGeoLoc() + "]";

//		return "GeoLocationContainer [geolocations=" + geoLocations + "]";
    }
}
