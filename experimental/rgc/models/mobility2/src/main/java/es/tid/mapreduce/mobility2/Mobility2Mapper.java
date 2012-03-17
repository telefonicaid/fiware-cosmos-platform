package es.tid.mapreduce.mobility2;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URI;
import java.util.HashMap;
import java.util.Map;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.mapreduce.mobility2.data.CdrUserData;
import es.tid.mapreduce.mobility2.data.CellData;
import es.tid.mapreduce.mobility2.data.Mobility2Counter;
import es.tid.mapreduce.mobility2.data.UserMobilityData;

/**
 * This class parses the CDRs files, and emit one value for each line, this line
 * is used for check that the calls have been done by users that we have their
 * data. For doing it we used Reduce-Side Joins. The format of file is: TODO
 * rgc: I dont know the meaning of all fields
 * <Phone_caller>|<Phone_receptor|....
 * 
 * Example:
 * <ol>
 * <li>Input: {key{line},value :
 * 33F4303E978D89|4531232401|33F4303E978D89|0454531232404|2|01
 * /06/2010|11:16:47|115|RMNACIO}</li>
 * <li>output(1): { key : {primary:1090066531, secondary:0}, values {} }</li>
 * </ol>
 * 
 * @author rgc
 */
public class Mobility2Mapper extends
        Mapper<LongWritable, Text, Text, UserMobilityData> {
    private Map<String, CellData> cacheCells;

    /**
     * @param key
     *            is the byte offset of the current line in the file;
     * @param value
     *            is the line from the file
     * @param context
     *            has the method "write()" to output the key,value pair
     */
    @Override
    public void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {

        CdrUserData cdrUserData;
        try {
            cdrUserData = CdrUserData.parse(value.toString());
        } catch (IllegalArgumentException e) {
            context.getCounter(Mobility2Counter.LINE_PARSER_CDRS_ERROR)
                    .increment(1L);
            return;
        }
        UserMobilityData umd = generateUserMobilityDataByCdr(cdrUserData);
        context.write(new Text(cdrUserData.getUserId()), umd);
    }

    /**
     * Method that reads the configuration and prepair the date validators.
     * 
     * @param context
     *            contains the context of the job run
     */
    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        Configuration conf = context.getConfiguration();
        this.cacheCells = new HashMap<String, CellData>();

        URI[] pathCaches = DistributedCache.getCacheFiles(context
                .getConfiguration());
        if (pathCaches != null) {
            for (int i = 0; i < pathCaches.length; i++) {
                loadCacheCells(new Path(pathCaches[i]), conf);
            }
        }
    }

    /**
     * Method that loads the cache with the data of the cells
     * 
     * @param path
     *            A path array of localized caches
     * @param conf
     *            Configuration that contains the localized archives
     * @throws IOException
     */
    private void loadCacheCells(Path path, Configuration conf)
            throws IOException {
        FSDataInputStream in = FileSystem.get(conf).open(path);
        BufferedReader br = new BufferedReader(new InputStreamReader(in));
        String line;
        try {
            while ((line = br.readLine()) != null) {
                CellData cellData = CellData.parse(line);
                this.cacheCells.put(cellData.getCellId(), cellData);
            }
        } finally {
            br.close();
        }
    }

    /**
     * 
     * @param cdrUserData
     * @return
     */
    private UserMobilityData generateUserMobilityDataByCdr(
            CdrUserData cdrUserData) {
        String bts = "";
        CellData cellData;
        if ((cellData = this.cacheCells.get(cdrUserData.getInitCellId())) != null) {
            bts = cellData.getBts();
        } else if ((cellData = this.cacheCells.get(cdrUserData.getEndCellId())) != null) {
            bts = cellData.getBts();
        }
        return new UserMobilityData(bts, cdrUserData.getDateCall());
    }
}