/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.kpicalculation.cleaning;

/**
 * This interface should be implemented by any filter to be applied to the web
 * profiling CDR's data
 *
 * @author javierb
 */
public interface IKpiCalculationFilter {
    /**
     * Method that filters an URL passed as parameter. If the url passes the
     * defined filter nothing happens, and the execution of the program
     * continues. If the URL does not pass the filter, then an exception is
     * thrown to indicate that the URL is forbidden as input of web profiling
     * module
     *
     * @param url
     *            url the filter will be applied to.
     */
    void filter(String url);
}
