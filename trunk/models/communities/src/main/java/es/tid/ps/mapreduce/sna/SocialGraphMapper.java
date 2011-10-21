package es.tid.ps.mapreduce.sna;

import java.io.IOException;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

/**
 * 
 * This class makes the first phase of the SNA algorithm, it recives with key
 * the name of the file that it is processing and the line in where there is,
 * and the value it is the line into the file. The format of this line is:
 * <ol>
 * <li>Counter: it is a internal counter into the format file.</li>
 * <li>Phone: it is the number of telephone that it makes the call.</li>
 * <li>Phone: it is the number of telephone that it recives the call.</li>
 * <li>Rest:</li>
 * <li>Example: 1|619555666|619777888|01/09/08 10:09:20|34|4|2</li>
 * </ol>
 * The delimiter of the fields is the pipe (|). <br>
 * The mapper emit two key/value pairs. One with the phone number of the caller
 * with key and phone number of the receiver. The other pair has the phone
 * number of the receiver with key and the phone number of the caller with
 * value.
 * <ol>
 * <li>Input: 1|619555666|619777888|01/09/08 10:09:20|34|4|2</li>
 * <li>output(1): { 619555666, 619777888}</li>
 * <li>output(2): { 619777888, 619555666}.</li>
 * </ol>
 * 
 * @author rgc@tid.es
 */
public class SocialGraphMapper extends Mapper<LongWritable, Text, Text, Text> {

    public static String DELIMITER = "\\|";

    /**
     * @param key
     *            is the byte offset of the current line in the file;
     * @param value
     *            is the line from the file
     * @param output
     *            has the method "collect()" to output the key,value pair
     */
    @Override
    public void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        String[] listLine = value.toString().split(DELIMITER, -1);
        context.write(new Text(listLine[1]), new Text(listLine[2]));
        context.write(new Text(listLine[2]), new Text(listLine[1]));
    }
}
