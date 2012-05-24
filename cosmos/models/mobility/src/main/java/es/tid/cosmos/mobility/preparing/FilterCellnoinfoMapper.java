package es.tid.cosmos.mobility.preparing;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 * Input: <Long, Cdr>
 * Output: <Long, Cdr>
 * 
 * @author dmicol
 */
public class FilterCellnoinfoMapper extends Mapper<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>> {
    @Override
    public void map(LongWritable key, ProtobufWritable<MobData> value,
                    Context context) throws IOException, InterruptedException {
        value.setConverter(MobData.class);
        final Cdr cdr = value.get().getCdr();
        if (cdr.getCellId() != 0) {
            context.write(new LongWritable(cdr.getCellId()), value);
        } else {
            context.write(key, value);
        }
    }
}
