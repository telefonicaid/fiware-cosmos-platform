package es.tid.cosmos.mobility.parsing;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.CellUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TelMonth;
import es.tid.cosmos.mobility.data.TelMonthUtil;

/**
 *
 * @author dmicol
 */
public class JoinBtsNodeToTelMonthAndCellReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, ProtobufWritable<TelMonth>,
        ProtobufWritable<Cell>> {
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
                int weekday = cdr.getDate().getWeekday();
                int hour = cdr.getTime().getHour();
                boolean workingday;
                if (weekday == 0 || weekday == 6 ||
                        (weekday == 5 && hour >= 18)) {
                    workingday = false;
                } else {
                    workingday = true;
                }
                ProtobufWritable<TelMonth> telMonth = TelMonthUtil.createAndWrap(
                        cdr.getUserId(), cdr.getDate().getMonth(), workingday);
                context.write(telMonth, CellUtil.wrap(cell));
            }
        }
    }
}
