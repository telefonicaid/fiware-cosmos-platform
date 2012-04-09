package es.tid.cosmos.kpicalculation.data;

import static org.junit.Assert.assertEquals;
import org.junit.Test;

/**
 *
 * @author dmicol
 */
public class KpiCalculationDataExceptionTest {
    private KpiCalculationDataException instance;
    
    @Test
    public void testConstructorWithCause() {
        String message = "Without cause exception";
        Throwable cause = new IllegalArgumentException();
        this.instance = new KpiCalculationDataException(
                message, cause, KpiCalculationCounter.LINE_FILTERED_3RDPARTY);
        assertEquals(message, this.instance.getMessage());
        assertEquals(cause, this.instance.getCause());
        assertEquals(KpiCalculationCounter.LINE_FILTERED_3RDPARTY,
                     this.instance.getCounter());
    }
    
    @Test
    public void testConstructorWithoutCause() {
        String message = "Without cause exception";
        this.instance = new KpiCalculationDataException(
                message, KpiCalculationCounter.LINE_FILTERED_PERSONAL_INFO);
        assertEquals(message, this.instance.getMessage());
        assertEquals(KpiCalculationCounter.LINE_FILTERED_PERSONAL_INFO,
                     this.instance.getCounter());
    }
}
