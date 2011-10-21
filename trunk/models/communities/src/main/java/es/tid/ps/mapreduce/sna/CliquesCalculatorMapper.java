package es.tid.ps.mapreduce.sna;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

/**
 * This class makes the groups for generate the close cliques in where the user
 * are into. For generate this groups, the map function emit one pair key/value
 * for each value into the array of the values,, this value is emited like key,
 * the key is mark like "principal" value, and the rest of the data are loaded
 * into the values array. <br>
 * Example: *
 * <ol>
 * <li>Input: {key: 1, values: 2,3,4}</li>
 * <li>output(1): { key : 2, values { principal : 1, values: [3, 4] } } }</li>
 * <li>output(2): { key : 3, values { principal : 1, values: [2, 4] } } }</li>
 * <li>output(3): { key : 4, values { principal : 1, values: [2, 3] } } }</li>
 * </ol>
 * 
 * @author rgc@tid.es
 */
public class CliquesCalculatorMapper extends
        Mapper<Text, ArrayListWritable, Text, NodeCombination> {

    /**
     * @param key
     *            it is the telephone number of one user ;
     * @param value
     *            are a array with the telephone number of all the people that
     *            the user has had a conference.
     * @param output
     *            has the method "collect()" to output the key,value pair
     */
    @Override
    public void map(Text key, ArrayListWritable value, Context context)
            throws IOException, InterruptedException {
        String keyStr = key.toString();
        for (Iterator<String> iterator = value.iterator(); iterator.hasNext();) {
            String string = iterator.next();
            List<String> list = new ArrayList<String>(value.getList());
            list.remove(string);
            NodeCombination nodeCombination = new NodeCombination(keyStr, list);
            context.write(new Text(string), nodeCombination);
        }
    }
}