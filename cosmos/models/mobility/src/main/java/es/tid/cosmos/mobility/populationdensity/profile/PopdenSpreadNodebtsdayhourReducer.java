package es.tid.cosmos.mobility.populationdensity.profile;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.NodeBtsDateUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDate;

/**
 *
 * @author ximo
 */
public class PopdenSpreadNodebtsdayhourReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, ProtobufWritable<NodeBtsDate>,
        NullWritable> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<Cdr> cdrList = new LinkedList<Cdr>();
        List<Cell> cellList = new LinkedList<Cell>();
        for (final ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getType()) {
                case CDR:
                    cdrList.add(mobData.getCdr());
                    break;
                case CELL:
                    cellList.add(mobData.getCell());
                    break;
                default:
                    throw new IllegalStateException("Invalid data type: "
                            + mobData.getType().toString());
            }
        }
        for (Cell catalogue : cellList) {
            for (Cdr cdr : cdrList) {
                context.write(
                        NodeBtsDateUtil.createAndWrap(cdr.getUserId(),
                                catalogue.getBts(), cdr.getDate(),
                                cdr.getTime().getHour()),
                        NullWritable.get());
            }
        }
    }
}
