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