package es.tid.cosmos.mobility.labelling.clientbts;

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
        List<BtsCounter> reprBtsCounterList = new LinkedList<BtsCounter>();
        List<BtsCounter> sumBtsCounterList = new LinkedList<BtsCounter>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getInputId()) {
                case 0:
                    sumBtsCounterList.add(mobData.getBtsCounter());
                    break;
                case 1:
                    reprBtsCounterList.add(mobData.getBtsCounter());
                    break;
                default:
                    throw new IllegalStateException("Unexpected MobData ID: "
                            + mobData.getInputId());
            }
        }
        
        key.setConverter(TwoInt.class);
        final TwoInt twoInt = key.get();
        for (BtsCounter reprBtsCounter : reprBtsCounterList) {
            for (BtsCounter sumBtsCounter : sumBtsCounterList) {
                ProtobufWritable<NodeBts> nodeBts = NodeBtsUtil.createAndWrap(
                        twoInt.getNum1(), (int)twoInt.getNum2(),
                        sumBtsCounter.getWeekday(), 0);
                ProtobufWritable<TwoInt> hourComs = TwoIntUtil.createAndWrap(
                        sumBtsCounter.getRange(), sumBtsCounter.getCount());
                context.write(nodeBts, hourComs);
            }
        }
    }
}
