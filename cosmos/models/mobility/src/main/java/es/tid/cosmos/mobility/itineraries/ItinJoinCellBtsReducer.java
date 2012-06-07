package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ItinTimeUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.util.CellsCatalogue;

/**
 * Input: <Long, Cdr>
 * Output: <TwoInt, ItinTime>
 * 
 * @author dmicol
 */
public class ItinJoinCellBtsReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, ProtobufWritable<TwoInt>,
        ProtobufWritable<MobData>> {
    private static List<Cell> cells;
    
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
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<Cell> filteredCells = CellsCatalogue.filter(cells, key.get());
        if (filteredCells.isEmpty()) {
            return;
        }
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            final Cdr cdr = mobData.getCdr();
            for (Cell cell : filteredCells) {
                final ProtobufWritable<TwoInt> nodeBts =
                        TwoIntUtil.createAndWrap(cdr.getUserId(),
                                                 cell.getBts());
                final ProtobufWritable<MobData> itTime =
                        MobDataUtil.createAndWrap(
                                ItinTimeUtil.create(cdr.getDate(),
                                                    cdr.getTime(),
                                                    cell.getBts()));
                context.write(nodeBts, itTime);
            }
        }
    }
}
