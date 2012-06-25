package es.tid.smartsteps.ipm;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.charset.Charset;
import java.util.LinkedList;
import java.util.List;

import es.tid.smartsteps.util.CsvParser;
import es.tid.smartsteps.util.CsvPrinter;

/**
 */
public abstract class RawToIpmConverterSupport<Raw, Ipm> extends
        AbstractRawToIpmConverter {

    protected RawToIpmConverterSupport(String delimiter, Charset charset) {
        super(delimiter, charset);
    }

    protected abstract Ipm convert(Raw raw);

    protected abstract CsvParser<Raw> newParser();

    protected abstract CsvPrinter<Ipm> newPrinter();

    private List<Ipm> convert(Iterable<Raw> fromObjs) {
        List<Ipm> result = new LinkedList<Ipm>();
        for (Raw raw : fromObjs) {
            result.add(convert(raw));
        }
        return result;
    }

    @Override
    public void convert(InputStream input, OutputStream output)
            throws IOException, ParseException {
        CsvParser<Raw> csvParser = this.newParser();
        CsvPrinter<Ipm> csvPrinter = this.newPrinter();

        List<Raw> parsedRaws = csvParser.parse(input);
        List<Ipm> convertedIpms = convert(parsedRaws);
        csvPrinter.print(convertedIpms, output);
    }
}
