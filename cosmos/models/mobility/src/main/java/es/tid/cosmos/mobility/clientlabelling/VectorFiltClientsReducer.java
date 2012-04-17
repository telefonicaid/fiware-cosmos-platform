package es.tid.cosmos.mobility.clientlabelling;

import java.util.HashSet;
import java.io.IOException;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class VectorFiltClientsReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<Cdr> cdrList = new LinkedList<Cdr>();
        int numCommsInfo = 0;
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getType()) {
                case CDR:
                    cdrList.add(mobData.getCdr());
                    break;
                case NODE_BTS_DAY:
                    numCommsInfo += mobData.getNodeBtsDay().getCount();
                    break;
                default:
                    throw new IllegalArgumentException();
            }
        }
        for (int i = 0; i < numCommsInfo; i++) {
            for (Cdr cdr : cdrs) {
                context.write(key,);
            }
        }
    }
}
