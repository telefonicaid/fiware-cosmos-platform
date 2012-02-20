package es.tid.ps.mobility.jobs;

import es.tid.ps.mobility.data.GassetProtocol;
import es.tid.ps.mobility.data.MxProtocol;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Mapper;

import java.io.IOException;

public class MobFromCellsMapper extends Mapper<IntWritable,
        GassetProtocol.GstCell, IntWritable, MxProtocol.MxCell> {

    @Override
    protected void map(IntWritable cell, GassetProtocol.GstCell gstCell,
                       Context context) throws IOException, InterruptedException {
        context.write(cell, MxProtocol.MxCell.newBuilder()
                .setCell(gstCell.getCell())
                .setBts(gstCell.getBts())
                .setMun(gstCell.getLac())
                .setState(gstCell.getState())
                .setPosx(gstCell.getPosx())
                .setPosy(gstCell.getPosy())
                .build());
    }
}
