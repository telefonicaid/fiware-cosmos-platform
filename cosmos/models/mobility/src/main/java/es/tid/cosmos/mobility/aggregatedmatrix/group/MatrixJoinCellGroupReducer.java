package es.tid.cosmos.mobility.aggregatedmatrix.group;

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MatrixTimeUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.generated.MobProtocol.CellGroup;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MatrixTime;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.util.CellGroupsCatalogue;

/**
 * Input: <Long, Cdr>
 * Output: <Long, MatrixTime>
 * 
 * @author dmicol
 */
public class MatrixJoinCellGroupReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>> {
    private static List<CellGroup> cellGroups;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        if (cellGroups == null) {
            final Configuration conf = context.getConfiguration();
            cellGroups = CellGroupsCatalogue.load(
                    new Path(conf.get("cell_groups")), conf);
        }
    }
    
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<CellGroup> filteredCellGroups = CellGroupsCatalogue.filter(
                cellGroups, key.get());
        if (filteredCellGroups.isEmpty()) {
            return;
        }
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            final Cdr cdr = mobData.getCdr();
            for (CellGroup cellGroup : filteredCellGroups) {
                final MatrixTime mtxTime = MatrixTimeUtil.create(cdr.getDate(),
                        cdr.getTime(), (int)cellGroup.getGroup().getNum2(),
                        cellGroup.getGroup().getNum1());
                context.write(new LongWritable(cdr.getUserId()),
                              MobDataUtil.createAndWrap(mtxTime));
            }
        }
    }
}
