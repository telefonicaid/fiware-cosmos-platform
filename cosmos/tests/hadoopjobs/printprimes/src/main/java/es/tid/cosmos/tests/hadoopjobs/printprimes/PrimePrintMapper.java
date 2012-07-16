package es.tid.cosmos.tests.hadoopjobs.printprimes;

import java.io.IOException;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

/**
 *
 * @author ximo
 */
public class PrimePrintMapper
        extends Mapper<LongWritable, Text, Text, IntWritable> {
    public static final Text KEY_COUNT = new Text("numprimes");
    public static final Text KEY_LIST = new Text("prime_list");
    private static final IntWritable ONE = new IntWritable(1);

    private static boolean isPrime(int num) {
        int div = 2;
        while(div < num && num % div != 0) {
            ++div;
        }
        return div == num;
    }

    @Override
    public void map(LongWritable keyin, Text value, Context context)
            throws IOException, InterruptedException {
        String[] tokens = value.toString().split("\\s"); // Split by whitespace
        for (String token : tokens) {
            int num = Integer.parseInt(token);
            if (PrimePrintMapper.isPrime(num)) {
                context.write(KEY_COUNT, ONE);
                context.write(KEY_LIST, new IntWritable(num));
            }
        }
    }
}
