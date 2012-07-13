package es.tid.smartsteps.ipm;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.charset.Charset;

/**
 *
 * @author dmicol
 */
public interface RawToIpmConverter {

    void convert(InputStream input, OutputStream output)
            throws IOException, ParseException;

    /**
     * Raw-to-IPM converter factory class.
     */
    interface Builder {

        RawToIpmConverter newConverter(String delimiter, Charset charset);
    }
}
