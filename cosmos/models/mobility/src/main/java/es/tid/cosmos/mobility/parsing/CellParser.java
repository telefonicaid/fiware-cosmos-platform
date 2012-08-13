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

package es.tid.cosmos.mobility.parsing;

import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;

/**
 *
 * @author sortega
 */
public class CellParser extends Parser {
    public CellParser(String line, String separator) {
        super(line, separator);
    }

    @Override
    public Cell parse() {
        try {
            return Cell.newBuilder()
                .setCellId(parseCellId())
                .setBts(parseLong())
                .setGeoloc1(parseInt())
                .setGeoloc2(parseInt())
                .setPosx(parseInt())
                .setPosy(parseInt())
                .build();
        } catch (Exception ex) {
            throw new IllegalArgumentException("Failed to parse: " + this.line,
                                               ex);
        }
    }
}
