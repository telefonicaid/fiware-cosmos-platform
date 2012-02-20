package es.tid.ps.mobility.jobs;

import java.io.IOException;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.ps.mobility.data.GassetProtocol.GstCell;
import es.tid.ps.mobility.data.MxProtocol.MxCell;

public class MobmxGetMobFromCellsMapper extends Mapper<IntWritable, GstCell,
        IntWritable, MxCell> {
    @Override
    protected void map(IntWritable cell, GstCell gstCell, Context context)
            throws IOException, InterruptedException {
        context.write(cell, MxCell.newBuilder()
                .setCell(gstCell.getCell())
                .setBts(gstCell.getBts())
                .setMun(gstCell.getLac())
                .setState(gstCell.getState())
                .setPosx(gstCell.getPosx())
                .setPosy(gstCell.getPosy())
                .build());
    }
}
