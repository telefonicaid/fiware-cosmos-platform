package es.tid.ps.kpicalculation;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.ps.kpicalculation.cleaning.ThirdPartyFilter;
import es.tid.ps.kpicalculation.cleaning.ExtensionFilter;
import es.tid.ps.kpicalculation.cleaning.KpiCalculationFilter;
import es.tid.ps.kpicalculation.cleaning.PersonalInfoFilter;

/**
 * 
 * This class makes the filtering of data in the cdrs received. Not Implemented
 * yet
 * 
 * @author javierb@tid.es
 */
public class KpiCleanerMapper extends Mapper<LongWritable, Text, LongWritable, Text> {

    public void map(LongWritable key, Text value, Context context) throws IOException, InterruptedException {

        //context.write(key, value);
        KpiCalculationFilter f = new ExtensionFilter().setNext(new ThirdPartyFilter()
                .setNext(new PersonalInfoFilter()));
        
        boolean emit = f.filter(value.toString());
        
        if (emit)
            context.write(key, value);

    }
}
