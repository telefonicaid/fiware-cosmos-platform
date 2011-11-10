package es.tid.mapreduce.mobility2;

import static es.tid.mapreduce.mobility2.data.RepresentVector.generateRepresentVector;

import java.io.IOException;
import java.net.URI;
import java.util.ArrayList;
import java.util.List;

import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.mapreduce.mobility2.data.BtsResume;
import es.tid.mapreduce.mobility2.data.Mobility2Counter;
import es.tid.mapreduce.mobility2.data.RepresentVector;
import es.tid.mapreduce.mobility2.data.UserMobilityData;

/**
 * This class makes the reduce phare in the generation of Mobility 2.0.
 * 
 * Their input is a Key with the identifier of the user and their values are all
 * the values aggregated during the mapping phase, these values contain the Cdrs
 * of the user.
 * 
 * This process generates a output with this format <br>
 * user_id vector_id bts_id pearson_measure
 * 
 * @author rgc
 */
public class Mobility2Reducer extends
        Reducer<Text, UserMobilityData, Text, Text> {

    UserMobilityHandler userMobilityHandler;
    List<String> indentifiers;

    /**
     * @param key
     *            is the key of the mapper. The key the identifier of the user
     * @param values
     *            are all the values aggregated during the mapping phase, these
     *            values contain the Cdrs of the user
     * @param context
     *            contains the context of the job run
     */
    @Override
    protected void reduce(Text key, Iterable<UserMobilityData> values,
            Context context) throws IOException, InterruptedException {
        try {
            List<BtsResume> resume = userMobilityHandler.processUser(values);
            writeResults(resume, key, context);
        } catch (RuntimeException e) {
            // TODO rgc: change to a define exception
            context.getCounter(Mobility2Counter.USER_CRDS_EXCEED).increment(1L);
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see
     * org.apache.hadoop.mapreduce.Reducer#setup(org.apache.hadoop.mapreduce
     * .Reducer.Context)
     */
    @Override
    public void setup(Context context) {
        try {
            indentifiers = new ArrayList<String>();
            List<RepresentVector> representVectors = new ArrayList<RepresentVector>();
            URI[] pathCaches = DistributedCache.getCacheFiles(context
                    .getConfiguration());
            if (pathCaches != null) {
                FileSystem fs = FileSystem.get(context.getConfiguration());
                for (int i = 0; i < pathCaches.length; i++) {
                    FSDataInputStream in = fs.open(new Path(pathCaches[i]));
                    RepresentVector aux = generateRepresentVector(in);
                    representVectors.add(aux);
                    indentifiers.add(aux.getName());
                }
            }
            userMobilityHandler = new UserMobilityHandler(representVectors);
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    /**
     * Method that write the output in a filesystem, it calculate the minimun
     * Pearson Measure for each PDI respect the representative vectors
     * 
     * @param resume
     *            a structure with the PDIs their mobility vector a the Pearson
     *            Measure
     * @param key
     *            the identifier of the user
     * @param context
     *            contains the context of the job run
     * @throws IOException
     * @throws InterruptedException
     */
    private void writeResults(List<BtsResume> resume, Text key, Context context)
            throws IOException, InterruptedException {
        for (String representVector : indentifiers) {
            Double min = Double.MAX_VALUE;
            String btsName = "";
            for (BtsResume btsResume : resume) {
                Double tmp = btsResume.getPearson().get(representVector);
                if (tmp != null && min > tmp) {
                    min = tmp;
                    btsName = btsResume.getBts();
                }
            }
            // TODO (rgc) reimplemente the ouput generator with StringBuilder or
            // something similar
            if (min != Double.MAX_VALUE){
            context.write(new Text(key.toString() + " " +  representVector + " " + btsName),
                    new Text(min.toString()));
            }
        }
    }
}
