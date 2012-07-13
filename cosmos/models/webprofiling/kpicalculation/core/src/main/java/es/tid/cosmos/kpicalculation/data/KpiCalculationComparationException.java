package es.tid.cosmos.kpicalculation.data;

import es.tid.cosmos.kpicalculation.data.KpiCalculationCounter;

/**
 * Extends RuntimeException and allows to notify mappers about errors in the
 * input data received.
 * 
 * @author javierb
 */
public class KpiCalculationComparationException extends RuntimeException {
    private static final long serialVersionUID = -8134324326773415951L;

    private KpiCalculationCounter counter;

    public KpiCalculationComparationException(String message, Throwable cause,
            KpiCalculationCounter wrongComparation) {
        super(message, cause);
        this.counter = wrongComparation;
    }

    public KpiCalculationComparationException(String message,
            KpiCalculationCounter wrongComparation) {
        super(message);
        this.counter = wrongComparation;
    }

    public KpiCalculationCounter getCounter() {
        return this.counter;
    }
}