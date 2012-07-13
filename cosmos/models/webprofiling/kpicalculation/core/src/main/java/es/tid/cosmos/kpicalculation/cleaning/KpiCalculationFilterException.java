package es.tid.cosmos.kpicalculation.cleaning;

import es.tid.cosmos.kpicalculation.data.KpiCalculationCounter;

/**
 * Extends RuntimeException and allows to notify mappers exceptions while
 * filtering data in web profiling module
 * 
 * @author javierb
 */
public class KpiCalculationFilterException extends RuntimeException {
    private static final long serialVersionUID = -2909950826083930318L;

    private KpiCalculationCounter counter;
    
    public KpiCalculationFilterException(String message, Throwable cause,
            KpiCalculationCounter lineFilteredExtension) {
        super(message, cause);
        this.counter = lineFilteredExtension;
    }

    public KpiCalculationFilterException(String message,
            KpiCalculationCounter lineFilteredExtension) {
        super(message);
        this.counter = lineFilteredExtension;
    }

    public KpiCalculationCounter getCounter() {
        return this.counter;
    }
}