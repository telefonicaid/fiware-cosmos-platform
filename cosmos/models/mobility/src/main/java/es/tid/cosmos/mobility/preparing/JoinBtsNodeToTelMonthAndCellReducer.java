package es.tid.cosmos.mobility.preparing;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.CellUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.MobProtocol.TelMonth;
import es.tid.cosmos.mobility.data.TelMonthUtil;
import es.tid.cosmos.mobility.util.CellsCatalogue;

/**
 *
 * @author dmicol
 */
public class JoinBtsNodeToTelMonthAndCellReducer extends Reducer<LongWritable,
        ProtobufWritable<Cdr>, ProtobufWritable<TelMonth>,
        ProtobufWritable<Cell>> {
    private static List<Cell> cells = null;
    
    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        if (cells == null) {
            final Configuration conf = context.getConfiguration();
            cells = CellsCatalogue.load(new Path(conf.get("cells")), conf);
        }
    }
    
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<Cdr>> values, Context context)
            throws IOException, InterruptedException {
        List<Cell> filteredCells = new LinkedList<Cell>();
        for (Cell cell : cells) {
            if (cell.getCellId() == key.get()) {
                filteredCells.add(cell);
            }
        }
        if (filteredCells.isEmpty()) {
            return;
        }
        for (ProtobufWritable<Cdr> value : values) {
            value.setConverter(Cdr.class);
            final Cdr cdr = value.get();
            for (Cell cell : filteredCells) {
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
