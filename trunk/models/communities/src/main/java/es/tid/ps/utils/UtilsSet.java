package es.tid.ps.utils;

import java.util.Set;
import java.util.TreeSet;

/**
 * Class in where there are some static methods to make operations over Set<T>
 * 
 * @author rgc
 */
public class UtilsSet {

    /**
     * Constructor
     */
    public UtilsSet() {
        super();
    }

    /**
     * Method that make the intersection of two sets A and B is the set that
     * contains all elements of A that also belong to B (or equivalently, all
     * elements of B that also belong to A), but no other elements.
     * 
     * @param setA
     *            firts set of elements
     * @param setB
     *            second set of elements
     * @return a set with the intersection
     */
    public static <T> Set<T> generateIntersection(Set<T> setA, Set<T> setB) {
        Set<T> tmp = new TreeSet<T>();
        for (T x : setA) {
            if (setB.contains(x)) {
                tmp.add(x);
            }
        }
        return tmp;
    }

    /**
     * Method that make the make of two sets A and B, The union of two sets A
     * and B is the collection of points which are in A or in B (or in both):
     * 
     * @param setA
     *            firts set of elements
     * @param setB
     *            second set of elements
     * @return a set with the union
     */
    public static <T> Set<T> generateUnion(Set<T> setA, Set<T> setB) {
        Set<T> tmp = new TreeSet<T>(setA);
        tmp.addAll(setB);
        return tmp;
    }

    /**
     * Method that make the make of two sets A and B, The difference of two sets
     * A and B is the collection of points which are in A and not in B.
     * 
     * @param setA
     *            firts set of elements
     * @param setB
     *            second set of elements
     * @return a set with the difference
     */
    public static <T> Set<T> generateDifference(Set<T> setA, Set<T> setB) {
        Set<T> tmp = new TreeSet<T>(setA);
        ;
        tmp.removeAll(setB);
        return tmp;
    }

    /**
     * Method that make the make of two sets A and B, The difference of two sets
     * A and B is the collection of points which are in A and not in B or are in
     * B and not in A:
     * 
     * @param setA
     *            firts set of elements
     * @param setB
     *            second set of elements
     * @return a set with the difference
     */
    public static <T> Set<T> generateSymmetricDifference(Set<T> setA,
            Set<T> setB) {
        Set<T> tmpA = generateDifference(setA, setB);
        Set<T> tmpB = generateDifference(setB, setA);
        tmpA.addAll(tmpB);
        return tmpA;
    }
}
