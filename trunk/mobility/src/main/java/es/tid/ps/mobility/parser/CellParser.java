package es.tid.ps.mobility.parser;

import es.tid.ps.mobility.data.MxProtocol.MxCell;

/**
 *
 * @author sortega
 */
public class CellParser extends PipeDelimitedParser {

    public CellParser(String line) {
        super(line);
    }

    @Override
    public MxCell parse() {
        return MxCell.newBuilder()
            .setCell(parseCell())
            .setBts(parseInt())
            .setMun(parseInt())
            .setState(parseInt())
            .setPosx(parseInt())
            .setPosy(parseInt())
            .build();
    }
}
