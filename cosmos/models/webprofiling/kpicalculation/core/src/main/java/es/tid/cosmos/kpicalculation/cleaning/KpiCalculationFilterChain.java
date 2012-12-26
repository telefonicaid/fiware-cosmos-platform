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

import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;

import org.apache.hadoop.conf.Configuration;

import es.tid.cosmos.kpicalculation.data.KpiCalculationCounter;

/**
 * Abstract class to implement the "chain of responsibility" pattern. This
 * pattern is used apply different filters to the input data, to asses if it
 * will be useful for calculating kpi's for the personalisation process.
 *
 * @author javierb
 *
 */
public class KpiCalculationFilterChain {
    private static final String COLLECTION_ID = "kpifilters";

    private List<IKpiCalculationFilter> handlers;

    public KpiCalculationFilterChain(Configuration conf) {
        this.handlers = new ArrayList<IKpiCalculationFilter>();
        Collection<String> classes = conf.getStringCollection(COLLECTION_ID);
        Iterator<String> it = classes.iterator();
        while (it.hasNext()) {
            IKpiCalculationFilter filter;
            try {
                filter = (IKpiCalculationFilter) Class
                        .forName(it.next().trim())
                        .getConstructor(Configuration.class).newInstance(conf);
                this.handlers.add(filter);
            } catch (Exception exception) {
                throw new KpiCalculationFilterException(
                        "Wrong configuration of filter", exception,
                        KpiCalculationCounter.WRONG_FILTER_CONFIGURATION);
            }
        }
    }

    public void filter(String url) {
        IKpiCalculationFilter currentFilter;
        ListIterator<IKpiCalculationFilter> it = this.handlers.listIterator();
        while (it.hasNext()) {
            currentFilter = it.next();
            currentFilter.filter(url);
        }
    }
}
