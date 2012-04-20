package es.tid.cosmos.mobility.activityarea;

import java.io.IOException;
import java.lang.Math;
import java.util.Set;
import java.util.HashSet;
import java.util.List;
import java.util.ArrayList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobVarsUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.MobProtocol.MobVars;
import es.tid.cosmos.mobility.data.MobProtocol.TelMonth;
import es.tid.cosmos.mobility.activityarea.Accumulations;

public class ActivityAreaReducer extends Reducer<
        ProtobufWritable<TelMonth>, ProtobufWritable<Cell>,
        LongWritable, ProtobufWritable<MobVars>> {
    private Set<Long> allCells;
    private Set<Long> allBtss;
    private Set<Integer> allMuns;
    private Set<Integer> allStates;
    private List<Cell> cellsWithDifBts;

    private Accumulations accumulate(Iterable<ProtobufWritable<Cell>> values) {
        Accumulations ans = new Accumulations();
        int numPos = 0;
        double massCenterAccX = 0.0;
        double massCenterAccY = 0.0;
        double radiusAccX = 0.0;
        double radiusAccY = 0.0;
        boolean hasNewBts;
        for (ProtobufWritable<Cell> value : values) {
            value.setConverter(Cell.class);
            Cell cell = value.get();
            numPos += 1;
            this.allCells.add(cell.getCellId());
            hasNewBts = this.allBtss.add(cell.getPlaceId());
            if (hasNewBts) {
                this.cellsWithDifBts.add(cell);
            }
            this.allMuns.add(cell.getGeoloc1());
            this.allStates.add(cell.getGeoloc2());
            massCenterAccX += cell.getPosx();
            massCenterAccY += cell.getPosy();
            radiusAccX += (cell.getPosx() * cell.getPosx());
            radiusAccY += (cell.getPosy() * cell.getPosy());
        }
        ans.difPos = this.allCells.size();
        ans.numBtss = this.allBtss.size();
        ans.numMuns = this.allMuns.size();
        ans.numStates = this.allStates.size();
        ans.massCenterX = massCenterAccX / numPos;
        ans.massCenterY = massCenterAccY / numPos;
        double radiusX =
            radiusAccX / numPos - (ans.massCenterX * ans.massCenterX);
        double radiusY =
            radiusAccY / numPos - (ans.massCenterY * ans.massCenterY);
        ans.radius = Math.sqrt(radiusX + radiusY);
        return ans;
    }

    private double getMaxDistance(List<Cell> cellsWithDifBts) {
        double maxDist = 0.0;
        for (int pos = 0; pos < cellsWithDifBts.size(); pos++) {
            Cell currentCell = cellsWithDifBts.get(pos);
            for(int further = pos + 1; further < cellsWithDifBts.size();
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

    @Override
    protected void reduce(ProtobufWritable<TelMonth> key,
            Iterable<ProtobufWritable<Cell>> values, Context context)
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

        Accumulations accs = accumulate(values);

        double influenceAreaDiameter = getMaxDistance(cellsWithDifBts);

        ProtobufWritable<MobVars> ans =
            MobVarsUtil.createAndWrap(month, isWorkDay, accs.difPos,
                                           accs.numBtss, accs.numMuns,
                                           accs.numStates, accs.massCenterX,
                                           accs.massCenterY, accs.radius,
                                           influenceAreaDiameter);
        context.write(newKey, ans);
    }
}
