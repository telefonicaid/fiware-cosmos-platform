package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ItinTimeUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class ItinJoinCellBtsReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, ProtobufWritable<TwoInt>,
        ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<Cdr> cdrList = new LinkedList<Cdr>();
        List<Cell> cellList = new LinkedList<Cell>();
        for (ProtobufWritable<MobData> value : values) {
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
                    throw new IllegalStateException("Unexpected MobData type: "
                            + mobData.getType().name());
            }
        }
        for (Cell cell : cellList) {
            for (Cdr cdr : cdrList) {
                final ProtobufWritable<TwoInt> nodeBts =
                        TwoIntUtil.createAndWrap(cdr.getUserId(),
                                                 cell.getPlaceId());
                final ProtobufWritable<MobData> itTime =
                        MobDataUtil.createAndWrap(
                                ItinTimeUtil.create(cdr.getDate(),
                                                    cdr.getTime(),
                                                    cdr.getCellId()));
                context.write(nodeBts, itTime);
            }
        }
    }
}
