package es.tid.smartsteps.footfalls.catchments;

import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.Catchment;
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.Catchments;
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.TopCell;
import es.tid.smartsteps.footfalls.data.generated.LookupProtocol.Lookup;

/**
 *
 * @author sortega
 */
public final class CatchmentUtil {

    private CatchmentUtil() {}

    /**
     * Scale the counts of Catchments according to a lookup and replace the
     * cell id.
     *
     * @param a       Catchment to be scaled
     * @param lookup  Scaling proportion and new cell id.
     * @return        Scaled Catchments
     */
    public static Catchments scaleCatchments(Catchments a, Lookup lookup) {
        final double proportion = lookup.getProportion();
        Catchments.Builder scaledCatchments = Catchments.newBuilder(a);
        scaledCatchments.setId(lookup.getValue());
        for (int i = 0; i < scaledCatchments.getCatchmentsCount(); i++) {
            final Catchment scaledCatchment = scaledCatchments.getCatchments(i);
            Catchment.Builder catchmentBuilder = Catchment.newBuilder();
            catchmentBuilder.setHour(scaledCatchment.getHour());
            for (int j = 0; j < scaledCatchment.getTopCellsCount(); j++) {
                final TopCell topCell = scaledCatchment.getTopCells(j);
                TopCell.Builder topCellBuilder = TopCell.newBuilder(topCell);
                topCellBuilder.setCount(topCellBuilder.getCount() * proportion);
                catchmentBuilder.addTopCells(topCellBuilder);
            }
            scaledCatchments.setCatchments(i, catchmentBuilder);
        }
        return scaledCatchments.build();
    }
}
