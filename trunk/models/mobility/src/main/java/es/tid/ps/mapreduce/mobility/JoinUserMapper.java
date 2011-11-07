package es.tid.ps.mapreduce.mobility;

import java.io.IOException;
import java.util.StringTokenizer;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.ps.mapreduce.mobility.data.CompositeKey;
import es.tid.ps.mapreduce.mobility.data.FileType;
import es.tid.ps.mapreduce.mobility.data.MobilityCounter;

/**
 * This class parses the file of Users into the platform, and emit one value for
 * each user, this line is used for check that the callers have been done by
 * users that we have their data. For doing it we used Reduce-Side Joins, in
 * where the Key is a composite key of the user_id and a identifier of the file
 * type. Example:
 * <ol>
 * <li>Input:{key{filename@line},value 1090066531|P|17/04/2010||||||15%|||||NO
 * INFORMADO|-4|-4}</li>
 * <li>output(1): { key : {primary:1090066531, secondary:0}, values {} }</li>
 * </ol>
 * 
 * @author rgc
 */
public class JoinUserMapper extends
        Mapper<LongWritable, Text, CompositeKey, Text> {
    final private static String DELIMITER = "\\|";

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.hadoop.mapreduce.Mapper#map(KEYIN, VALUEIN,
     * org.apache.hadoop.mapreduce.Mapper.Context)
     */
    @Override
    public void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        StringTokenizer st = new StringTokenizer(value.toString(), DELIMITER);
        if (st.hasMoreElements()) {
            context.write(
                    new CompositeKey(st.nextToken(), FileType.FILE_USERS
                            .getValue()), new Text(""));
        } else {
            context.getCounter(MobilityCounter.LINE_PARSER_USER_ERROR)
                    .increment(1L);
        }
    }
}
