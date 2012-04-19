package es.tid.cosmos.mobility.labelling.clientbts;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class VectorFiltClientbtsReducer extends Reducer<ProtobufWritable<TwoInt>,
        ProtobufWritable<BtsCounter>, ProtobufWritable<NodeBts>,
        ProtobufWritable<TwoInt>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<BtsCounter>> values, Context context)
            throws IOException, InterruptedException {
        List<BtsCounter> reprBtsCounterList = new LinkedList<BtsCounter>();
        List<BtsCounter> sumBtsCounterList = new LinkedList<BtsCounter>();
        for (ProtobufWritable<BtsCounter> value : values) {
            value.setConverter(BtsCounter.class);
            final BtsCounter counter = value.get();
            if (counter.getWeekday() == -1 && counter.getRange() == -1) {
                reprBtsCounterList.add(counter);
            } else {
                sumBtsCounterList.add(counter);
            }
        }
        
        key.setConverter(TwoInt.class);
        final TwoInt twoInt = key.get();
        for (BtsCounter sumBtsCounter : sumBtsCounterList) {
            for (BtsCounter reprBtsCounter : reprBtsCounterList) {
                ProtobufWritable<NodeBts> nodeBts = NodeBtsUtil.createAndWrap(
                        twoInt.getNum1(), (int)twoInt.getNum2(),
                        reprBtsCounter.getWeekday(), 0);
                ProtobufWritable<TwoInt> hourComs = TwoIntUtil.createAndWrap(
                        reprBtsCounter.getRange(), reprBtsCounter.getCount());
                context.write(nodeBts, hourComs);
            }
        }
    }
}
