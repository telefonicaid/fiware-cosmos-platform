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

import java.text.MessageFormat;
import java.util.StringTokenizer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.hadoop.conf.Configuration;

/**
 * @author javierb
 */
public abstract class AbstractKpiCalculationFilter implements
        IKpiCalculationFilter {
    private static final String FILTER_PARAMETER_DELIMITER = "\n";

    private Pattern pattern;
    private Matcher matcher;

    protected void init(Configuration conf, String configParameter,
            String regularExpression) {
        String forbiddenPattern = setPattern(regularExpression,
                                             conf.get(configParameter));
        this.pattern = Pattern.compile(forbiddenPattern,
                                       Pattern.CASE_INSENSITIVE);
    }

    @Override
    public abstract void filter(String url);

    /**
     * Builds the regExp pattern that will filter input URL's using a predefined
     * regExp template specific of each filter and the values to be filtered
     * that will be in a config file
     *
     * @param regExp
     *            Basic template of the regExp
     * @param values
     *            String of values to be filtered
     * @return Final regExp to be applied
     */
    protected String setPattern(String regExp, String values) {
        StringTokenizer stt = new StringTokenizer(values.trim(),
                FILTER_PARAMETER_DELIMITER);
        StringBuilder filters = new StringBuilder("(");
        while (stt.hasMoreTokens()) {
            filters.append(stt.nextToken().trim().replace(".", "\\.")).append(
                    "|");
        }
        filters.setCharAt(filters.length() - 1, ')');
        return MessageFormat.format(regExp, filters.toString());
    }

    protected Pattern getPattern() {
        return this.pattern;
    }

    protected void setPattern(Pattern pattern) {
        this.pattern = pattern;
    }

    protected Matcher getMatcher() {
        return this.matcher;
    }

    protected void setMatcher(Matcher matcher) {
        this.matcher = matcher;
    }
}
