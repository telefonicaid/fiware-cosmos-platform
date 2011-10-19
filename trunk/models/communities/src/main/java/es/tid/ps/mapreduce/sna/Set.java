package es.tid.ps.mapreduce.sna;

/**
 * Class to storage the vertex
 * 
 * @author rgc
 * 
 */
public class Set {

    public int[] vertex;
    public int size;

    public Set(int size) {
        this.size = 0;
        vertex = new int[size];
    }

    /**
     * Static method in where we make a copy set. It is necesary to check
     * because this method modifies the second param and return it
     * 
     * @param from
     *            source of the data to copy
     * @param to
     *            target object in where load the data
     * @return a copy of the first object set
     */
    static public Set clone(final Set from, Set to) {
        to.size = from.size;
        for (int i = 0; i < from.size; ++i) {
            to.vertex[i] = from.vertex[i];
        }
        return to;
    }
}