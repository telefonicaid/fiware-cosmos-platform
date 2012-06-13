package es.tid.cosmos.mobility.aggregatedmatrix.group;

import java.io.IOException;
import java.util.List;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MatrixTimeUtil;
import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.generated.MobProtocol.CellGroup;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MatrixTime;
import es.tid.cosmos.mobility.util.CellGroupsCatalogue;

/**
 * Input: <Long, Cdr>
 * Output: <Long, MatrixTime>
 * 
 * @author dmicol
 */
public class MatrixJoinCellGroupReducer extends Reducer<LongWritable,
        TypedProtobufWritable<Cdr>, LongWritable, TypedProtobufWritable<MatrixTime>> {
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
            Iterable<TypedProtobufWritable<Cdr>> values, Context context)
            throws IOException, InterruptedException {
        List<CellGroup> filteredCellGroups = CellGroupsCatalogue.filter(
                cellGroups, key.get());
        if (filteredCellGroups.isEmpty()) {
            return;
        }
        for (TypedProtobufWritable<Cdr> value : values) {
            final Cdr cdr = value.get();
            for (CellGroup cellGroup : filteredCellGroups) {
                final MatrixTime mtxTime = MatrixTimeUtil.create(cdr.getDate(),
                        cdr.getTime(), (int)cellGroup.getGroup().getNum2(),
                        cellGroup.getGroup().getNum1());
                context.write(new LongWritable(cdr.getUserId()),
                              new TypedProtobufWritable<MatrixTime>(mtxTime));
            }
        }
    }
}
