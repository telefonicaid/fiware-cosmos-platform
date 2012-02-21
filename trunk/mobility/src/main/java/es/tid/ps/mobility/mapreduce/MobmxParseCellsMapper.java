package es.tid.ps.mobility.mapreduce;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.ps.mobility.data.MxProtocol.MxCdr;
import es.tid.ps.mobility.data.MxProtocol.MxCell;
import es.tid.ps.mobility.parsing.CellParser;

/**
 *
 * @author sortega
 */
public class MobmxParseCellsMapper extends Mapper<IntWritable, Text, LongWritable,
        ProtobufWritable<MxCdr>> {

    @Override
    protected void map(IntWritable lineno, Text line, Context context)
            throws IOException, InterruptedException {
        final MxCell cell = new CellParser(line.toString()).parse();
        ProtobufWritable wrappedCdr = ProtobufWritable.newInstance(MxCell.class);
        wrappedCdr.set(cell);
        context.write(new LongWritable(cell.getCell()), wrappedCdr);
    }
}
