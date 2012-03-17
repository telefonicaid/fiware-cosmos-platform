package es.tid.ps.mapreduce.sna;

import java.util.Set;
import java.util.TreeSet;

/**
 * This class is for order the vertex in base of the number of neighbors that it
 * has.
 * 
 * @author rgc
 */
class VertexNeigbors implements Comparable<VertexNeigbors> {
    // the identifier of the vertex in the graph
    private final Integer position;
    // the numnber of the neighbors from the vertex in the graph
    private final Integer neighbors;

    /**
     * Constructor
     * 
     * @param position
     *            the identifier of the vertex in the graph
     * @param neighbors
     *            the numnber of the neighbors from the vertex in the graph
     */
    public VertexNeigbors(Integer position, Integer neighbors) {
        this.position = position;
        this.neighbors = neighbors;
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Comparable#compareTo(java.lang.Object)
     */
    @Override
    public int compareTo(VertexNeigbors o) {
        if (this.neighbors > o.neighbors) {
            return 1;
        } else if (this.neighbors < o.neighbors) {
            return -1;
        } else {
            return (this.position.compareTo(o.position));
        }
    }

    /**
     * Method that generate the vertex set from the set of VertexNeigbors
     * 
     * @param vertexNeigborsSet
     *            set of VertexNeigbors object where the set is got
     * @return a set of vertex indetifier (Integer)
     */
    public static Set<Integer> getVertexSet(
            Set<VertexNeigbors> vertexNeigborsSet) {
        Set<Integer> vertexSet = new TreeSet<Integer>();

        for (VertexNeigbors vertexNeigbors : vertexNeigborsSet) {
            vertexSet.add(vertexNeigbors.position);
        }
        return vertexSet;
    }
}