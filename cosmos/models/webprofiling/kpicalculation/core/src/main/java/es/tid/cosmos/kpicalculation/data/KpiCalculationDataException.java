package es.tid.cosmos.kpicalculation.data;

import es.tid.cosmos.kpicalculation.data.KpiCalculationCounter;

/**
 * Extends RuntimeException and allows to notify mappers about errors in the
 * input data received.
 *
 * @author javierb
 */
public class KpiCalculationDataException extends RuntimeException {
    private static final long serialVersionUID = -8134324326773415950L;

    private KpiCalculationCounter counter;

    public KpiCalculationDataException(String message, Throwable cause,
            KpiCalculationCounter lineFilteredExtension) {
        super(message, cause);
        this.counter = lineFilteredExtension;
    }

    public KpiCalculationDataException(String message,
            KpiCalculationCounter lineFilteredExtension) {
        super(message);
        this.counter = lineFilteredExtension;
    }

    public KpiCalculationCounter getCounter() {
        return this.counter;
    }
}