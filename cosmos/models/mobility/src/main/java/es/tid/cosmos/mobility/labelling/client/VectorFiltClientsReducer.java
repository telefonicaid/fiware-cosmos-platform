package es.tid.cosmos.mobility.labelling.client;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.CdrUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class VectorFiltClientsReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<Cdr>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<Integer> ncommsList = new LinkedList<Integer>();
        List<Cdr> cdrList = new LinkedList<Cdr>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getType()) {
                case INT:
                    ncommsList.add(mobData.getInt());
                    break;
                case CDR:
                    cdrList.add(mobData.getCdr());
                    break;
                default:
                    throw new IllegalArgumentException();
            }
        }
        for (Integer ncomms : ncommsList) {
            for (Cdr cdr : cdrList) {
                context.write(key, CdrUtil.wrap(cdr));
            }
        }
    }
}
