package es.tid.ps.kpicalculation.cleaning;

/**
 * Abstract class to implement the "chain of responsibility" pattern. This
 * pattern is used apply different filters to the input data, to asses if it
 * will be useful for calculating kpi's for the personalisation process.
 * 
 * @author javierb
 * 
 */
public abstract class KpiCalculationFilter {

    protected KpiCalculationFilter next;

    public KpiCalculationFilter setNext(KpiCalculationFilter filter) {
        next = filter;
        return this;
    }

    public abstract boolean filter(String s);
}
