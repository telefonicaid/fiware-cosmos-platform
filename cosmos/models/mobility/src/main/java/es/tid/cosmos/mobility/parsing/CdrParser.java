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

import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;

/**
 *
 * @author sortega
 */
class CdrParser extends Parser {
    private final DateParser dateParser;

    public CdrParser(String line, String separator, DateParser dateParser) {
        super(line, separator);
        this.dateParser = dateParser;
    }

    @Override
    public Cdr parse() {
        try {
            Cdr.Builder cdr = Cdr.newBuilder();

            cdr.setCellId(this.parseCellId()); // Cell 1
            cdr.setUserId(this.parseUserId()); // Phone 1
            if (cdr.getCellId() == 0) { // Cell 2
                cdr.setCellId(this.parseCellId());
            } else {
                this.nextToken();
            }
            this.nextToken(); // Phone 2
            this.nextToken(); // Unknown field

            cdr.setDate(this.dateParser.parse(this.nextToken()));
            cdr.setTime(this.parseTime());

            return cdr.build();
        } catch (Exception ex) {
            throw new IllegalArgumentException("Failed to parse: " + this.line,
                                               ex);
        }
    }
}
