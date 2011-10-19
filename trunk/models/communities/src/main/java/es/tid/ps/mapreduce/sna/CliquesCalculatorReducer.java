package es.tid.ps.mapreduce.sna;

import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

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
 * generate de matrix Generate de cliques
 * <br>
 * Interate another time the values and compliated the matix
 * <br>
 * Generate de cliques {7,6,8} {4,7} <br>
 * Emit the key/values { {5,7,6,8}, ""} {{5,4,7},}
 * 
 * @author rgc
 * 
 */

public class CliquesCalculatorReducer extends Reducer<Text, NodeCombination, Text, Text> {

    private LinkedList<String> names;
    private String pKey;

    protected void reduce(Text key, Iterable<NodeCombination> values, Context context) throws IOException,
            InterruptedException {
        names = new LinkedList<String>();
        pKey = key.toString();

        Map<Integer, List<String>> processValues = new HashMap<Integer, List<String>>();

        int cont = 0;
        for (Iterator<NodeCombination> iterator = values.iterator(); iterator.hasNext();) {
            NodeCombination value = iterator.next();
            names.add(value.getPrincipal());
            processValues.put(cont, value.getValues());
            ++cont;
        }

        boolean[][] connected = new boolean[cont][cont];
        for (Iterator<Entry<Integer, List<String>>> iterator = processValues.entrySet().iterator(); iterator.hasNext();) {
            Entry<Integer, List<String>> entry = iterator.next();
            int keyPosition = entry.getKey();
            List<String> list = entry.getValue();
            connected[keyPosition][keyPosition] = true;
            for (String string : list) {
                int position = names.indexOf(string);
                if (position != -1) {
                    connected[position][keyPosition] = true;
                    connected[keyPosition][position] = true;
                }
            }
        }

        int[] all = new int[connected.length];
        for (int c = 0; c < connected.length; c++) {
            all[c] = c;
        }

        version2(connected, all, 0, connected.length, new Set(connected.length), new Set(connected.length), context);
    }

    /**
     * TODO Copy from "url" it is necessary to review all of them
     * 
     * @param adjMatrix
     * @param oldMD
     * @param oldTestedSize
     * @param oldCandidateSize
     * @param actualMD
     * @param best
     * @param context
     * @throws IOException
     * @throws InterruptedException
     */
    private void version2(boolean[][] adjMatrix, int[] oldMD, int oldTestedSize, int oldCandidateSize, Set actualMD,
            Set best, Context context) throws IOException, InterruptedException {
        int[] actualCandidates = new int[oldCandidateSize];
        int nod;
        int fixp = 0;
        int actualCandidateSize;
        int actualTestedSize;
        int i;
        int j;
        int count;
        int pos = 0;
        int p;
        int s = 0;
        int sel;
        int index2Tested;
        boolean fini = false;

        index2Tested = oldCandidateSize;
        nod = 0;

        // Determine each counter value and look for minimum
        // Branch and bound step
        // Is there a node in ND (represented by MD and index2Tested)
        // which is connected to all nodes in the candidate list CD
        // we are finished and backtracking will not be enabled
        for (i = 0; (i < oldCandidateSize) && (index2Tested != 0); ++i) {
            p = oldMD[i];
            count = 0;

            // Count disconnections
            for (j = oldTestedSize; (j < oldCandidateSize) && (count < index2Tested); ++j) {
                if (adjMatrix[p][oldMD[j]] == false) {
                    count++;

                    // Save position of potential candidate
                    pos = j;
                }
            }

            // Test new minimum
            if (count < index2Tested) {
                fixp = p;
                index2Tested = count;

                if (i < oldTestedSize) {
                    s = pos;
                } else {
                    s = i;

                    // preincr
                    nod = 1;
                }
            }
        }

        // If fixed point initially chosen from candidates then
        // number of diconnections will be preincreased by one
        // Backtracking step for all nodes in the candidate list CD
        for (nod = index2Tested + nod; nod >= 1; --nod) {
            // Interchange
            p = oldMD[s];
            oldMD[s] = oldMD[oldTestedSize];
            sel = oldMD[oldTestedSize] = p;

            // Fill new set "not"
            actualCandidateSize = 0;

            for (i = 0; i < oldTestedSize; ++i) {
                if (adjMatrix[sel][oldMD[i]] != false) {
                    actualCandidates[actualCandidateSize++] = oldMD[i];
                }
            }

            // Fill new set "candidates"
            actualTestedSize = actualCandidateSize;

            for (i = oldTestedSize + 1; i < oldCandidateSize; ++i) {
                if (adjMatrix[sel][oldMD[i]] != false) {
                    actualCandidates[actualTestedSize++] = oldMD[i];
                }
            }

            // Add to "actual relevant nodes"
            actualMD.vertex[actualMD.size++] = sel;

            // so CD+1 and ND+1 are empty
            if (actualTestedSize == 0) {
                if (best.size < actualMD.size) {
                    // found a max clique
                    Set.clone(actualMD, best);
                }

                int[] tmpResult = new int[actualMD.size];
                System.arraycopy(actualMD.vertex, 0, tmpResult, 0, actualMD.size);
                addClique(tmpResult, context);
            } else {
                if (actualCandidateSize < actualTestedSize) {
                    version2(adjMatrix, actualCandidates, actualCandidateSize, actualTestedSize, actualMD, best,
                            context);
                }
            }

            if (fini) {
                break;
            }

            // move node from MD to ND
            // Remove from compsub
            actualMD.size--;

            // Add to "nod"
            oldTestedSize++;

            if (nod > 1) {
                // Select a candidate disconnected to the fixed point
                for (s = oldTestedSize; adjMatrix[fixp][oldMD[s]] != false; ++s) {
                }
            }
            // end selection
        }

        // Backtrackcycle
        actualCandidates = null;
    }

    /**
     * Method that emit the clique, it convert the positions into the array to
     * telephone number, concatenate these data with "," and emit a key/value
     * with all the data in the key
     * 
     * @param array
     *            vector with the positions into the matrix that are in a clique
     * @param context
     * @throws IOException
     * @throws InterruptedException
     */
    protected void addClique(int[] array, Context context) throws IOException, InterruptedException {
        // count and store only defined cliques
        StringBuilder sb = new StringBuilder(pKey);
        for (int i = 0; i < array.length; i++) {
            sb.append(",");
            sb.append(names.get(array[i]));
        }
        context.write(new Text(sb.toString()), new Text(""));
    }
}
