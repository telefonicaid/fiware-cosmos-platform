package es.tid.bdp.mobility.mapreduce;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.bdp.mobility.data.GassetProtocol.GstCell;
import es.tid.bdp.mobility.data.MobProtocol.Cell;

public class GetMobFromCellsMapper extends Mapper<IntWritable,
        ProtobufWritable<GstCell>, IntWritable, ProtobufWritable<Cell>> {
    @Override
    public void map(IntWritable key, ProtobufWritable<GstCell> value,
            Context context) throws IOException, InterruptedException {
        GstCell gstCell = value.get();
        Cell cell = Cell.newBuilder()
                .setCellId(gstCell.getCellId())
                .setPlaceId(gstCell.getPlaceId())
                .setGeoloc1(gstCell.getLac())
                .setGeoloc2(gstCell.getState())
                .setPosx(gstCell.getPosx())
                .setPosy(gstCell.getPosy())
                .build();
        ProtobufWritable<Cell> wrapper = ProtobufWritable.newInstance(
                Cell.class);
        wrapper.set(cell);
        context.write(key, wrapper);
    }
}
