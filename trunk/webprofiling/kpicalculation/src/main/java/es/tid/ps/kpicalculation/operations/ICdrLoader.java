package es.tid.ps.kpicalculation.operations;

/**
 * This interface should be implemented by any class used to load cdr data that
 * will be used to calculate aggregates for web profiling module
 * 
 * @author javierb
 * 
 */
public interface ICdrLoader {
    int load(String folder);
}
