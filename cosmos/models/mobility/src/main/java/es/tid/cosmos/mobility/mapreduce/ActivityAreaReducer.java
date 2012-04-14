package es.tid.cosmos.mobility.mapreduce;

import java.io.IOException;
import java.lang.Math;
import java.util.Set;
import java.util.HashSet;
import java.util.List;
import java.util.ArrayList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ActivityAreaUtil;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityArea;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityAreaKey;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;

/**
 *
 * @author losa
 */

class Accumulations {
    int difPos;
    int numBtss;
    int numMuns;
    int numStates;
    double massCenterX;
    double massCenterY;
    double radius;

    Accumulations() {
        difPos = 0;
        numBtss = 0;
        numMuns = 0;
        numStates = 0;
        massCenterX = 0.0;
        massCenterY = 0.0;
        radius = 0.0;
    }
}

public class ActivityAreaReducer extends Reducer<
        ProtobufWritable<ActivityAreaKey>, ProtobufWritable<Cell>,
        ProtobufWritable<ActivityAreaKey>, ProtobufWritable<ActivityArea>> {
    private Set<Long> allCells;
    private Set<Long> allBtss;
    private Set<Integer> allMuns;
    private Set<Integer> allStates;
    private List<Cell> cellsWithDifBts;

    protected Accumulations accumulate(Iterable<ProtobufWritable<Cell>> values) {
        Accumulations ans = new Accumulations();
        int numPos = 0;
        double massCenterAccX = 0.0;
        double massCenterAccY = 0.0;
        double radiusAccX = 0.0;
        double radiusAccY = 0.0;
        boolean already_seen;
        for (ProtobufWritable<Cell> value : values) {
            value.setConverter(Cell.class);
            Cell cell = value.get();
            numPos += 1;
            allCells.add(cell.getCellId());
            already_seen = allBtss.add(cell.getPlaceId());
            if (!already_seen) {
                cellsWithDifBts.add(cell);
            }
            allMuns.add(cell.getGeoloc1());
            allStates.add(cell.getGeoloc2());
            massCenterAccX += cell.getPosx();
            massCenterAccY += cell.getPosy();
            radiusAccX += (massCenterAccX * massCenterAccX);
            radiusAccY += (massCenterAccY * massCenterAccY);
        }
        ans.difPos = allCells.size();
        ans.numBtss = allBtss.size();
        ans.numMuns = allMuns.size();
        ans.numStates = allStates.size();
        ans.massCenterX = massCenterAccX / numPos;
        ans.massCenterY = massCenterAccY / numPos;
        double radiusX = radiusAccX / numPos - ans.massCenterX * ans.massCenterX;
        double radiusY = radiusAccY / numPos - ans.massCenterY * ans.massCenterY;
        ans.radius = Math.sqrt((radiusX + radiusY)/ numPos);
        return ans;
    }

    protected double getMaxDistance(List<Cell> cellsWithDifBts) {
        double maxDist = 0.0;
        for (int pos = 0; pos < cellsWithDifBts.size(); pos++) {
            Cell currentCell = cellsWithDifBts.get(pos);
            for(int further = pos + 1; further < cellsWithDifBts.size();
                    further++) {
                Cell furtherCell = cellsWithDifBts.get(further);
                double contribX = currentCell.getPosx() - furtherCell.getPosx();
                double contribY = currentCell.getPosy() - furtherCell.getPosy();
                double currentDist = Math.sqrt((contribX * contribX) +
                                               (contribY * contribY));
                if (currentDist > maxDist) {
                    maxDist = currentDist;
                }
            }
        }
        return maxDist;
    }

    @Override
    protected void reduce(ProtobufWritable<ActivityAreaKey> key,
            Iterable<ProtobufWritable<Cell>> values, Context context)
            throws IOException, InterruptedException {
        this.allCells = new HashSet<Long>();
        this.allBtss = new HashSet<Long>();
        this.allMuns = new HashSet<Integer>();
        this.allStates = new HashSet<Integer>();
        this.cellsWithDifBts = new ArrayList<Cell>();

        Accumulations accs = accumulate(values);

        double influenceAreaDiameter = getMaxDistance(cellsWithDifBts);

        ProtobufWritable<ActivityArea> ans =
            ActivityAreaUtil.createAndWrap(accs.difPos, accs.numBtss,
                                           accs.numMuns, accs.numStates,
                                           accs.massCenterX, accs.massCenterY,
                                           accs.radius, influenceAreaDiameter);
        context.write(key, ans);
    }
}
