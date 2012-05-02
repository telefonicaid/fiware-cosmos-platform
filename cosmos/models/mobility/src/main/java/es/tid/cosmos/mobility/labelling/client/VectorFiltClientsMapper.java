package es.tid.cosmos.mobility.labelling.client;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.CdrUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class VectorFiltClientsMapper extends Mapper<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<Cdr>> {
    @Override
    protected void map(LongWritable key, ProtobufWritable<MobData> value,
            Context context) throws IOException, InterruptedException {
        value.setConverter(MobData.class);
        final MobData mobData = value.get();
        switch (mobData.getType()) {
            case INT:
                context.getCounter(Counters.NUM_COMMS.name(),
                                   String.valueOf(key.get())).increment(1L);
                break;
            case CDR:
                context.write(key, CdrUtil.wrap(mobData.getCdr()));
                break;
            default:
                throw new IllegalArgumentException();
        }
    }
}
