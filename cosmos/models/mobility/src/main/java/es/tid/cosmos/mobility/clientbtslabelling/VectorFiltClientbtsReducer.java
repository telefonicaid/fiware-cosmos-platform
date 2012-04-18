package es.tid.cosmos.mobility.clientbtslabelling;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class VectorFiltClientbtsReducer extends Reducer<ProtobufWritable<TwoInt>,
        ProtobufWritable<MobData>, ProtobufWritable<NodeBts>,
        ProtobufWritable<TwoInt>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<Integer> ncommsList = new LinkedList<Integer>();
        List<BtsCounter> btsCounterList = new LinkedList<BtsCounter>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getType()) {
                case INT:
                    ncommsList.add(mobData.getInt());
                    break;
                case BTS_COUNTER:
                    btsCounterList.add(mobData.getBtsCounter());
                    break;
                default:
                    throw new IllegalArgumentException();
            }
        }
        
        key.setConverter(TwoInt.class);
        final TwoInt twoInt = key.get();
        for (Integer ncomms : ncommsList) {
            for (BtsCounter btsCounter : btsCounterList) {
                ProtobufWritable<NodeBts> nodeBts = NodeBtsUtil.createAndWrap(
                        twoInt.getNum1(), (int)twoInt.getNum2(),
                        btsCounter.getWeekday(), 0);
                ProtobufWritable<TwoInt> hourComs = TwoIntUtil.createAndWrap(
                        btsCounter.getRange(), btsCounter.getCount());
                context.write(nodeBts, hourComs);
            }
        }
    }
}
