package es.tid.mapreduce.mobility2.data;

import java.util.Arrays;
import java.util.Calendar;
import java.util.List;

/**
 * This class is a wrapper of a array of doubles, it has methods to get, and set
 * their values. And it has another functions for calculate the pearson measure
 * and for normalize their values.
 * 
 * @author rgc
 */
public class Vector {
    final double[] vector;

    /**
     * Constructor
     * 
     * @param capacity
     *            the capacity of the vector
     */
    public Vector(int capacity) {
        vector = new double[capacity];
    }

    /**
     * This method is a specidication normalizations that it is necesary in the
     * process of mobility.
     */
    public void preNormalizeByMobility() {
        for (int i = 0; i < 24; ++i) {
            vector[i] /= 4;
        }
    }

    /**
     * This method converted to unit vectors.Normalizing a vector involves two
     * steps: <br>
     * 1 calculate its length, then <br>
     * 2 divide each of its (xy or xyz) components by its length.
     */
    public void normalize() {
        double sumatory = 0;
        for (int i = 0; i < vector.length; ++i) {
            sumatory += vector[i];
        }
        for (int i = 0; i < vector.length; ++i) {
            vector[i] /= sumatory;
        }
    }

    /**
     * Increments the element at the specified position in this list in one
     * 
     * @param index
     *            index of element to replace.
     * @throws IndexOutOfBoundsException
     *             - if the index is out of range (index < 0 || index >=
     *             size()).
     */
    public void incrementPosition(int position) {
        this.vector[position]++;
    }

    /**
     * Returns the number of elements in this list
     * 
     * @return the number of elements in this list.
     */
    public int getSize() {
        return this.vector.length;
    }

    /**
     * Returns the element at the specified position in this list.
     * 
     * @param index
     *            index of element to return
     * @return the element at the specified position in this list
     * @throws IndexOutOfBoundsException
     *             - if the index is out of range (index < 0 || index >=
     *             size()).
     */
    public double getItem(int index) throws IndexOutOfBoundsException {
        return this.vector[index];
    }

    /**
     * Replaces the element at the specified position in this list with the
     * specified element (optional operation).
     * 
     * @param index
     *            index of element to replace.
     * @param value
     *            element to be stored at the specified position
     * @throws IndexOutOfBoundsException
     *             - if the index is out of range (index < 0 || index >=
     *             size()).
     */
    public void set(int index, double value) throws IndexOutOfBoundsException {
        this.vector[index] = value;
    }

    /**
     * Method that calculates a pearson correlation between the value of vector
     * an another vector that recives by param
     * 
     * @param target
     *            the vector with which we make the correlation
     * @return the result of the pearson measure
     */
    public double getPearsonCorrelation(Vector target) {
        double result = 0;
        double sum_sq_x = 0;
        double sum_sq_y = 0;
        double sum_coproduct = 0;
        double mean_x = this.vector[0];
        double mean_y = target.vector[0];
        for (int i = 2; i < this.vector.length + 1; i += 1) {
            double sweep = Double.valueOf(i - 1) / i;
            double delta_x = this.vector[i - 1] - mean_x;
            double delta_y = target.vector[i - 1] - mean_y;
            sum_sq_x += delta_x * delta_x * sweep;
            sum_sq_y += delta_y * delta_y * sweep;
            sum_coproduct += delta_x * delta_y * sweep;
            mean_x += delta_x / i;
            mean_y += delta_y / i;
        }
        double pop_sd_x = (double) Math.sqrt(sum_sq_x / this.vector.length);
        double pop_sd_y = (double) Math.sqrt(sum_sq_y / this.vector.length);
        double cov_x_y = sum_coproduct / this.vector.length;
        result = cov_x_y / (pop_sd_x * pop_sd_y);
        return result;
    }
 
    /* (non-Javadoc)
     * @see java.lang.Object#hashCode()
     */
    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + Arrays.hashCode(vector);
        return result;
    }

    /* (non-Javadoc)
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
        Vector other = (Vector) obj;
        if (!Arrays.equals(vector, other.vector))
            return false;
        return true;
    }

    /**
     * Static method that generates a vector from a list with date of cdrs
     * 
     * @param dateList
     *            list of date of cdrs
     * @return a mobility vector
     */
    public static Vector generateVector(List<Calendar> dateList) {
        Vector vector = new Vector(96);
        for (Calendar cal : dateList) {
            int date = cal.get(Calendar.DAY_OF_WEEK);
            int hour = cal.get(Calendar.HOUR_OF_DAY);
            switch (date) {
            case 1:
            case 2:
            case 3:
            case 4:
                vector.incrementPosition(hour);
                break;
            case 5:
                vector.incrementPosition(24 + hour);
                break;
            case 6:
                vector.incrementPosition(2 + 24 + hour);
                break;
            case 0:
                vector.incrementPosition(3 * 24 + hour);
                break;
            default:
                break;
            }
        }
        return vector;
    }
}
