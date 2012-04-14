package es.tid.cosmos.mobility.preparing;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.CdrUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class JoinBtsNodeToCdrReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<Cdr>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<Cdr> cdrs = new LinkedList<Cdr>();
        List<Cell> cells = new LinkedList<Cell>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            if (mobData.hasCdr()) {
                cdrs.add(mobData.getCdr());
            } else if (mobData.hasCell()) {
                cells.add(mobData.getCell());
            } else {
                throw new IllegalArgumentException("Invalid input data");
            }
        }
        
        if (cells.isEmpty()) {
            for (Cdr cdr : cdrs) {
                context.write(new LongWritable(cdr.getCellId()),
                              CdrUtil.wrap(cdr));
            }
        }
    }
}
