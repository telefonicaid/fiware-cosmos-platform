package es.tid.cosmos.mobility.labelling.client;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.Config;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 * Input: <Long, Int|Cdr>
 * Output: <Long, Cdr>
 * 
 * @author dmicol
 */
public class VectorFiltClientsReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>> {
    private int maxCdrs;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final Configuration conf = context.getConfiguration();
        this.maxCdrs = conf.getInt(Config.MAX_CDRS, Integer.MAX_VALUE);
    }
    
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        boolean hasComms = false;
        List<Cdr> cdrList = new LinkedList<Cdr>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getType()) {
                case INT:
                    hasComms = true;
                    break;
                case CDR:
                    cdrList.add(mobData.getCdr());
                    break;
                default:
                    throw new IllegalArgumentException();
            }
            if (hasComms) {
                break;
            }
            if (cdrList.size() > this.maxCdrs) {
                return;
            }
        }
        if (!hasComms) {
            // If we don't have communications for this user, we won't output
            // any CDRs
            return;
        }
        for (Cdr cdr : cdrList) {
            context.write(key, MobDataUtil.createAndWrap(cdr));
        }
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            final Cdr cdr = mobData.getCdr();
            if (mobData.getType() != MobData.Type.CDR) {
                throw new IllegalStateException();
            }
            context.write(key, MobDataUtil.createAndWrap(cdr));
        }
    }
}
