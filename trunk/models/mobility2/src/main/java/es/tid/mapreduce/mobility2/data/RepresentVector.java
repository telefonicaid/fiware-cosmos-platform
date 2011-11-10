package es.tid.mapreduce.mobility2.data;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

/**
 * Pojo that storages the mobility vector an a identifier of this vector
 * 
 * @author rgc
 */
public class RepresentVector {
    final private String name;
    final Vector vector;

    /**
     * Constructor
     * 
     * @param name
     *            identifier of the mobility vector
     * @param vector
     *            mobility vector
     */
    public RepresentVector(String name, Vector vector) {
        this.name = name;
        this.vector = vector;
    }

    /**
     * @return the vector
     */
    public Vector getVector() {
        return vector;
    }

    /**
     * @return the name
     */
    public String getName() {
        return name;
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
        result = prime * result + ((name == null) ? 0 : name.hashCode());
        result = prime * result + ((vector == null) ? 0 : vector.hashCode());
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
        RepresentVector other = (RepresentVector) obj;
        if (name == null) {
            if (other.name != null)
                return false;
        } else if (!name.equals(other.name))
            return false;
        if (vector == null) {
            if (other.vector != null)
                return false;
        } else if (!vector.equals(other.vector))
            return false;
        return true;
    }

    /**
     * Method that load the representative vectors with their date
     * 
     * @param in
     *            is a reference to source of data
     * @return a Pojo that storages the mobility vector an a identifier of this
     *         vector
     * @throws IOException
     */
    public static RepresentVector generateRepresentVector(InputStream in)
            throws IOException {
        BufferedReader br = new BufferedReader(new InputStreamReader(in));
        String name = br.readLine();
        Vector vector = new Vector(96);
        try {
            String linea;
            int i = 0;
            while ((linea = br.readLine()) != null) {
                vector.set(i, Double.parseDouble(linea));
                ++i;
            }
        } finally {
            br.close();
        }
        return new RepresentVector(name, vector);
    }
}
