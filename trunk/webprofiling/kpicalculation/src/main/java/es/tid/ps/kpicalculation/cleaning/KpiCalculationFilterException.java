package es.tid.ps.kpicalculation.cleaning;

import es.tid.ps.kpicalculation.data.KpiCalculationCounter;

/**
 * Extends RuntimeException and allows to notify mappers exceptions while
 * filtering data in web profiling module
 *  
 * @author javierb
 */
public class KpiCalculationFilterException extends RuntimeException {

    private static final long serialVersionUID = 1L;
    
    public KpiCalculationFilterException(String message, Throwable cause,
            KpiCalculationCounter lineFilteredExtension) {
        super(message, cause);
        counter = lineFilteredExtension;
    }

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