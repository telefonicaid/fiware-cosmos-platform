package es.tid.ps.kpicalculation.data;

import es.tid.ps.kpicalculation.data.KpiCalculationCounter;

/**
 * Extends RuntimeException and allows to notify mappers about errors in the
 * input data received.
 * 
 * @author javierb
 */
public class KpiCalculationDataException extends RuntimeException {

    public KpiCalculationDataException(String message, Throwable cause,
            KpiCalculationCounter lineFilteredExtension) {
        super(message, cause);
        counter = lineFilteredExtension;
    }

    private KpiCalculationCounter counter;

    public KpiCalculationDataException(String message,
            KpiCalculationCounter lineFilteredExtension) {
        super(message);
        counter = lineFilteredExtension;
    }

    public KpiCalculationCounter getCounter() {
        return counter;
    }

}