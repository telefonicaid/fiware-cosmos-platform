package es.tid.cosmos.mobility.populationdensity.profile;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.NodeBtsDateUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.util.CellsCatalogue;

/**
 *
 * @author ximo
 */
public class PopdenSpreadNodebtsdayhourReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, ProtobufWritable<MobData>,
        NullWritable> {
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
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        for (final ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            Cdr cdr = mobData.getCdr();
            for (Cell cell : cells) {
                context.write(
                        MobDataUtil.createAndWrap(NodeBtsDateUtil.create(
                                cdr.getUserId(), cell.getBts(),
                                cdr.getDate(), cdr.getTime().getHour())),
                        NullWritable.get());
            }
        }
    }
}
