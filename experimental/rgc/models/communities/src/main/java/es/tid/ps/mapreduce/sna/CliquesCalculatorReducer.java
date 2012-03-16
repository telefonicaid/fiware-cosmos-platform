package es.tid.ps.mapreduce.sna;

import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;
import java.util.SortedSet;
import java.util.TreeSet;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.ps.utils.UtilsSet;

/**
 * This class generate the cliques from each of the key with all of its values.
 * In the process firts we need the read all the principals values for generate
 * the size of the combination matrix, then we iterate another time all the
 * data, and activate the positions that appears into the values, and to
 * conclude calculate the cliques of this matrix and emit a key value foreach
 * that clique adding in each emit the original key. <br>
 * Process: <br>
 * input: {@code key:5 , values[ p: 4, v:[7]},{p: 6, v:[7,8]},{p: 7,
 * v:[4,6,8]},{p: 8, v:[6,7]}] } <br>
 * generate de matrix Generate de cliques <br>
 * Interate another time the values and compliated the matix <br>
 * Generate de cliques {7,6,8} {4,7} <br>
 * Emit the key/values { {5,7,6,8}, ""} {{5,4,7},}
 * 
 * @author rgc
 */

public class CliquesCalculatorReducer extends
        Reducer<Text, NodeCombination, Text, Text> {
    // List with the name of the vertix. The indentification is their position
    // in this list
    private List<String> names;
    // The key that are checking their cliques
    private String pKey;
    // The matrix with the social grafh
    private Map<Integer, Set<Integer>> matrix;

    /**
     * @param key
     *            is the key of the mapper
     * @param values
     *            are all the values aggregated during the mapping phase
     * @param context
     *            contains the context of the job run
     */
    protected void reduce(Text key, Iterable<NodeCombination> values,
            Context context) throws IOException, InterruptedException {
        names = new LinkedList<String>();
        pKey = key.toString();

        Map<Integer, List<String>> processValues = new HashMap<Integer, List<String>>();
        matrix = new HashMap<Integer, Set<Integer>>();
        SortedSet<VertexNeigbors> vertexNeigborsSet = new TreeSet<VertexNeigbors>();

        // count for indentifier each vertex with a Integer
        Integer nodeIdentifier = 0;
        for (Iterator<NodeCombination> iterator = values.iterator(); iterator
                .hasNext();) {
            NodeCombination value = iterator.next();
            names.add(value.getPrincipal());
            processValues.put(nodeIdentifier, value.getValues());
            ++nodeIdentifier;
        }

        for (Iterator<Entry<Integer, List<String>>> iterator = processValues
                .entrySet().iterator(); iterator.hasNext();) {
            Entry<Integer, List<String>> entry = iterator.next();
            Set<Integer> phoneNumberList = new TreeSet<Integer>();
            Integer neighbors = 0;
            Integer position = entry.getKey();
            for (String phoneNumber : entry.getValue()) {
                int neighborsPosition = names.indexOf(phoneNumber);
                if (neighborsPosition != -1) {
                    phoneNumberList.add(neighborsPosition);
                    ++neighbors;
                }
            }
            matrix.put(entry.getKey(), phoneNumberList);
            vertexNeigborsSet.add(new VertexNeigbors(position, neighbors));
        }
        bronKerbosch3(vertexNeigborsSet, context);
    }

    /**
     * method for improving the basic form of the Bron–Kerbosch algorithm
     * involves forgoing pivoting at the outermost level of recursion, and
     * instead choosing the ordering of the recursive calls carefully in order
     * to minimize the sizes of the sets of candidate vertices within each
     * recursive call.
     * http://en.wikipedia.org/wiki/Bron%E2%80%93Kerbosch_algorithm
     * #With_vertex_ordering
     * 
     * @param vertexNeigborsSet
     *            Set of vertex order by the number of neigbors that they have
     * @param context
     *            contains the context of the job run
     * @throws IOException
     * @throws InterruptedException
     */
    private void bronKerbosch3(Set<VertexNeigbors> vertexNeigborsSet,
            Context context) throws IOException, InterruptedException {
        Set<Integer> candidateSet = new TreeSet<Integer>();
        Set<Integer> processSet = new TreeSet<Integer>();
        Set<Integer> vertexSet = VertexNeigbors.getVertexSet(vertexNeigborsSet);

        for (Integer candidateVertex : vertexSet) {
            Set<Integer> tmpSet;
            Set<Integer> candidateVertexSet = new TreeSet<Integer>();
            candidateVertexSet.add(candidateVertex);
            Set<Integer> aNeigbors = getNeighbors(candidateVertex);
            bronKerbosch2(
                    UtilsSet.generateUnion(candidateSet, candidateVertexSet),
                    UtilsSet.generateIntersection(vertexSet, aNeigbors),
                    UtilsSet.generateIntersection(processSet, aNeigbors),
                    context);
            tmpSet = UtilsSet.generateDifference(vertexSet, candidateVertexSet);
            vertexSet = tmpSet;
            tmpSet = UtilsSet.generateUnion(processSet, candidateVertexSet);
            processSet = tmpSet;
        }
    }

    /**
     * This method implements the Bron–Kerbosch with pivoting, in the algorithm
     * firts choose the "pivot vertex" $vertex, chosen from $vertexSet ⋃
     * $processSet). The we need the neighbors of the $vertex. Therefore, only
     * $vertex and its non-neighbors need to be tested as the choices for the
     * vertex that is added to $candidateSet in each recursive call to the
     * algorithm. <br>
     * http://en.wikipedia.org/wiki/Bron%E2%80%93Kerbosch_algorithm#
     * With_pivoting
     * 
     * @param candidateSet
     *            set with the nodes catidates to be in the clique
     * @param vertexSet
     *            set with the nodes that are not ckeck too.
     * @param processSet
     *            set of nodes where has been checking
     * @param context
     *            contains the context of the job run
     * @throws IOException
     * @throws InterruptedException
     */
    private void bronKerbosch2(Set<Integer> candidateSet,
            Set<Integer> vertexSet, Set<Integer> processSet, Context context)
            throws IOException, InterruptedException {
        // if there are no value into vertexSet and processSet, then we have the
        // clique in candidateSet because there are no more vertex to check
        if (vertexSet.isEmpty() && processSet.isEmpty()) {
            addClique(candidateSet, context);
            return;
        }
        Integer vertex = chooseVertex(vertexSet, processSet);
        Set<Integer> vertexNeigbors = getNeighbors(vertex);

        Set<Integer> diffTmp = UtilsSet.generateDifference(vertexSet,
                vertexNeigbors);
        for (int candidateVertex : diffTmp) {
            Set<Integer> tmpSet;
            Set<Integer> candidateVertexSet = new TreeSet<Integer>();
            candidateVertexSet.add(candidateVertex);

            Set<Integer> aNeigbors = getNeighbors(candidateVertex);
            bronKerbosch2(
                    UtilsSet.generateUnion(candidateSet, candidateVertexSet),
                    UtilsSet.generateIntersection(vertexSet, aNeigbors),
                    UtilsSet.generateIntersection(processSet, aNeigbors),
                    context);
            
            // Normaly 'Assignment of parameter is not allowed' but we need to
            // change the value of the param (this is the razon why is not
            // final)
            tmpSet = UtilsSet.generateDifference(vertexSet, candidateVertexSet);
            vertexSet = tmpSet;
            
            // Normaly 'Assignment of parameter is not allowed' but we need to
            // change the value of the param (this is the razon why is not
            // final)
            tmpSet = UtilsSet.generateUnion(processSet, candidateVertexSet);
            processSet = tmpSet;
        }
    }

    /**
     * Method that choose the best node for start the interation
     * 
     * @param p
     *            set with the nodes that are not ckeck too
     * @param x
     *            set of nodes where has been checking
     * @return the node that has being choosen
     */
    private Integer chooseVertex(Set<Integer> vertexSet, Set<Integer> processSet) {
        // TODO rgc: it is necesary a better algorithm to select the vertex but
        // for this selection need work with VertexNeigbors instead of Integer
        // and choose the vertex with maximum number of neigbors. Ours solution
        // choose first the maxium of the vertex and them the maximum of the
        // process so for me is not too bad an approximation
        if (!vertexSet.isEmpty()) {
            return ((TreeSet<Integer>) vertexSet).last();
        } else {
            return ((TreeSet<Integer>) processSet).last();
        }
    }

    /**
     * Method that return the neighbors of one vertex in the dataset
     * 
     * @param i
     *            the identifier of the vertex
     * @return set with the vertex neighbors
     */
    private Set<Integer> getNeighbors(Integer i) {
        return matrix.get(i);
    }

    /**
     * Method that emit the clique, it convert identifier fo the vertex to
     * telephone number, concatenate these data with "," and emit a key/value
     * with all the data in the key (note: the first value is the key because
     * this always is part of the clique)
     * 
     * @param cliqueSet
     *            set with the identifier of the vertex in the clique
     * @param context
     *            contains the context of the job run
     * @throws IOException
     * @throws InterruptedException
     */
    private void addClique(Set<Integer> cliqueSet, Context context)
            throws IOException, InterruptedException {
        StringBuilder sb = new StringBuilder(pKey);
        for (Integer nodeIdentifier : cliqueSet) {
            sb.append(",");
            sb.append(names.get(nodeIdentifier));
        }
        context.write(new Text(sb.toString()), new Text(""));
    }
}
