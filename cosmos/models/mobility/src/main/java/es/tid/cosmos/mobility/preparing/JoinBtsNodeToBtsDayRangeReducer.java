package es.tid.cosmos.mobility.preparing;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class JoinBtsNodeToBtsDayRangeReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<TwoInt>> {
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
            return;
        }
        for (Cell cell : cells) {
            for (Cdr cdr : cdrs) {
                int group;
                switch (cdr.getDate().getWeekday()) {
                    case 0:
                        group = 3;
                        break;
                    case 5:
                        group = 1;
                        break;
                    case 6:
                        group = 2;
                        break;
                    default:
                        group = 0;
                }
                context.write(new LongWritable(cell.getPlaceId()),
                              TwoIntUtil.createAndWrap(group,
                                                       cdr.getTime().getHour()));
            }
        }
    }
}
