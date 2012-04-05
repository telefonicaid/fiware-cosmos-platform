package es.tid.bdp.mobility.mapreduce;

import java.io.IOException;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.bdp.mobility.data.GassetProtocol.GstCell;
import es.tid.bdp.mobility.data.MobProtocol.Cell;

public class GetMobFromCellsMapper extends Mapper<IntWritable, GstCell,
        IntWritable, Cell> {
    @Override
    protected void map(IntWritable cell, GstCell gstCell, Context context)
            throws IOException, InterruptedException {
        context.write(cell, Cell.newBuilder()
                .setCellId(gstCell.getCellId())
                .setPlaceId(gstCell.getPlaceId())
                .setGeoloc1(gstCell.getLac())
                .setGeoloc2(gstCell.getState())
                .setPosx(gstCell.getPosx())
                .setPosy(gstCell.getPosy())
                .build());
    }
}
