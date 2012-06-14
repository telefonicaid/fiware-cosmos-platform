package es.tid.cosmos.mobility.mivs;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobVarsUtil;
import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobVars;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TelMonth;

/**
 * Input: <TelMonth, Cell>
 * Output: <Long, MobVars>
 * 
 * @author logc
 */
class ActivityAreaReducer extends Reducer<
        ProtobufWritable<TelMonth>, TypedProtobufWritable<Cell>,
        LongWritable, TypedProtobufWritable<MobVars>> {
    private static class Accumulations {
        int difPos;
        int numBtss;
        int numMuns;
        int numStates;
        double massCenterX;
        double massCenterY;
        double radius;
    }
    
    private Set<Long> allCells;
    private Set<Long> allBtss;
    private Set<Integer> allMuns;
    private Set<Integer> allStates;
    private List<Cell> cellsWithDifBts;

    @Override
    protected void reduce(ProtobufWritable<TelMonth> key,
            Iterable<TypedProtobufWritable<Cell>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TelMonth.class);
        final LongWritable newKey = new LongWritable(key.get().getPhone());
        final int month = key.get().getMonth();
        final boolean isWorkDay = key.get().getWorkingday();
        this.allCells = new HashSet<Long>();
        this.allBtss = new HashSet<Long>();
        this.allMuns = new HashSet<Integer>();
        this.allStates = new HashSet<Integer>();
        this.cellsWithDifBts = new ArrayList<Cell>();

        Accumulations accs = this.accumulate(values);
        double influenceAreaDiameter = this.getMaxDistance(cellsWithDifBts);

        MobVars ans = MobVarsUtil.create(
                month, isWorkDay, accs.difPos, accs.numBtss, accs.numMuns,
                accs.numStates, accs.massCenterX, accs.massCenterY, accs.radius,
                influenceAreaDiameter);
        context.write(newKey, new TypedProtobufWritable<MobVars>(ans));
    }
    
    private Accumulations accumulate(
            Iterable<TypedProtobufWritable<Cell>> values) {
        Accumulations ans = new Accumulations();
        int numPos = 0;
        double massCenterAccX = 0.0D;
        double massCenterAccY = 0.0D;
        double radiusAccX = 0.0D;
        double radiusAccY = 0.0D;
        for (TypedProtobufWritable<Cell> value : values) {
            final Cell cell = value.get();
            numPos++;
            this.allCells.add(cell.getCellId());
            boolean hasNewBts = this.allBtss.add(cell.getBts());
            if (hasNewBts) {
                this.cellsWithDifBts.add(cell);
            }
            this.allMuns.add(cell.getGeoloc1());
            this.allStates.add(cell.getGeoloc2());
            massCenterAccX += cell.getPosx();
            massCenterAccY += cell.getPosy();
            radiusAccX += cell.getPosx() * cell.getPosx();
            radiusAccY += cell.getPosy() * cell.getPosy();
        }
        ans.difPos = this.allCells.size();
        ans.numBtss = this.allBtss.size();
        ans.numMuns = this.allMuns.size();
        ans.numStates = this.allStates.size();
        ans.massCenterX = massCenterAccX / numPos;
        ans.massCenterY = massCenterAccY / numPos;
        double radiusX =
            (radiusAccX / numPos) - (ans.massCenterX * ans.massCenterX);
        double radiusY =
            (radiusAccY / numPos) - (ans.massCenterY * ans.massCenterY);
        ans.radius = Math.sqrt(radiusX + radiusY);
        return ans;
    }

    private double getMaxDistance(List<Cell> cellsWithDifBts) {
        double maxDist = 0.0D;
        for (int pos = 0; pos < cellsWithDifBts.size(); pos++) {
            Cell currentCell = cellsWithDifBts.get(pos);
            for (int further = pos + 1; further < cellsWithDifBts.size();
                    further++) {
                Cell furtherCell = cellsWithDifBts.get(further);
                double contribX =
                    currentCell.getPosx() - furtherCell.getPosx();
                double contribY =
                    currentCell.getPosy() - furtherCell.getPosy();
                double currentDist = Math.sqrt(contribX * contribX +
                                               contribY * contribY);
                if (currentDist > maxDist) {
                    maxDist = currentDist;
                }
            }
        }
        return maxDist;
    }
}
