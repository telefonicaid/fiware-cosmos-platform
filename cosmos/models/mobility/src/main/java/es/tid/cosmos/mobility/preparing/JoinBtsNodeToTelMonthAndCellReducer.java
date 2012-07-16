package es.tid.cosmos.mobility.preparing;

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.conf.MobilityConfiguration;
import es.tid.cosmos.mobility.data.TelMonthUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TelMonth;
import es.tid.cosmos.mobility.util.CellsCatalogue;

/**
 * Input: <Long, Cdr>
 * Output: <TelMonth, Cell>
 *
 * @author dmicol
 */
class JoinBtsNodeToTelMonthAndCellReducer extends Reducer<LongWritable,
        TypedProtobufWritable<Cdr>, ProtobufWritable<TelMonth>,
        TypedProtobufWritable<Cell>> {
    private static List<Cell> cells = null;

    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        if (cells == null) {
            final MobilityConfiguration conf = new MobilityConfiguration(context.
                    getConfiguration());
            cells = CellsCatalogue.load(new Path(conf.get("cells")), conf);
        }
    }

    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<Cdr>> values, Context context)
            throws IOException, InterruptedException {
        List<Cell> filteredCells = CellsCatalogue.filter(cells, key.get());
        if (filteredCells.isEmpty()) {
            return;
        }
        for (TypedProtobufWritable<Cdr> value : values) {
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
                context.write(telMonth,
                              new TypedProtobufWritable<Cell>(cell));
            }
        }
    }
}
