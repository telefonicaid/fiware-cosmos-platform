package es.tid.ps.kpicalculation.cleaning;

import es.tid.ps.kpicalculation.data.KpiCalculationCounter;

public class KpiCalculationFilterException extends RuntimeException {

    private KpiCalculationCounter counter;

    public KpiCalculationFilterException(String message,
            KpiCalculationCounter lineFilteredExtension) {
        super(message);
        counter = lineFilteredExtension;
    }

    public KpiCalculationCounter getCounter() {
        return counter;
    }

}
