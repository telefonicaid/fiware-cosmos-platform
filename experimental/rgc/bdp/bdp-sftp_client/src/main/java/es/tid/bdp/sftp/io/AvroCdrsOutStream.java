package es.tid.bdp.sftp.io;

import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.avro.Schema;
import org.apache.avro.Schema.Field;
import org.apache.avro.Schema.Type;
import org.apache.avro.file.CodecFactory;
import org.apache.avro.file.DataFileWriter;
import org.apache.avro.generic.GenericData;
import org.apache.avro.generic.GenericDatumWriter;
import org.apache.avro.generic.GenericRecord;
import org.apache.avro.util.Utf8;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.pig.parser.ParserException;

import es.tid.bdp.utils.PropertiesPlaceHolder;

/**
 * This class serializes a stream and converts each bock into GenericAvro
 * objects. It breaks the buffer into lines with delimiter by default is
 * END_OF_LINE (\n) then, it parses the line into a avro object from the
 * definition given in the line configuration. This definition has the name of
 * the variable, and the type in each position of the cdr.
 * 
 * @author rgc
 * 
 */
public class AvroCdrsOutStream extends OutputStream {

    private static final String LINE_SEPARATOR = "\n";
    private static final String INPUT_ATTRS_AVRO = "input.attrs.avro";
    private static final String INPUT_ATTRS_REGEX = "input.attrs.regex";
    private static final String INPUT_ATTRS_DESC = "input.attrs.desc";

    private GenericRecord record;
    private StringBuilder linea;
    private Field[] parserList;
    private Pattern pattern;
    DataFileWriter<GenericRecord> file;

    /**
     * Constructor. Into the construct it loads the configuration from the
     * properties holder. It prepares the schema for generating the record and
     * the structure for parsing the line
     * 
     * @param out
     *            a stream for writing output.
     * @throws IOException
     */
    public AvroCdrsOutStream(FSDataOutputStream out) throws IOException {
        super();
        PropertiesPlaceHolder properties = PropertiesPlaceHolder.getInstance();
        Schema schema = new Schema.Parser().parse(new File(properties
                .getProperty(INPUT_ATTRS_AVRO)));

        file = new DataFileWriter<GenericRecord>(
                new GenericDatumWriter<GenericRecord>(schema));

        file.setMeta("version", 1);
        file.setMeta("creator", "tid");
        file.setCodec(CodecFactory.deflateCodec(5));
        file.create(schema, out);

        record = new GenericData.Record(schema);
        this.linea = new StringBuilder();
        this.pattern = Pattern.compile(properties
                .getProperty(INPUT_ATTRS_REGEX));
        prepareLineParser(properties.getProperty(INPUT_ATTRS_DESC), schema);
    }

    /**
     * Method prepares the parser of the cdr line to covert into a avro
     * GenericRecord
     * 
     * @param lineConfigurator
     *            string with the description of the cdr line
     */
    private void prepareLineParser(String lineConfigurator, Schema schema) {
        String[] array = lineConfigurator.split("\\|");
        this.parserList = new Field[array.length];

        for (int i = 0; i < array.length; i++) {
            this.parserList[i] = schema.getField(array[i]);
        }
    }

    /*
     * This method is not implemented
     */
    @Override
    public void write(int arg0) throws IOException {
        throw new RuntimeException("method not implemented");
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.io.OutputStream#write(byte[], int, int)
     */
    @Override
    public void write(byte[] buffer, int off, int len) throws IOException {
        linea.append(new Utf8(buffer), off, len);
        int offset = linea.indexOf(LINE_SEPARATOR);
        while (offset != -1) {
            String returnValue = linea.substring(0, offset);
            linea.delete(0, offset + 1);
            offset = linea.indexOf(LINE_SEPARATOR);
            try {
                parseLine(returnValue);
                file.append(record);
                // writer.write(record, encoder);

            } catch (Exception e) {
                // TODO: handle exception
                System.err.println("error: " + returnValue);
                System.in.read();
                e.printStackTrace();
            }
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.io.OutputStream#close()
     */
    @Override
    public void close() throws IOException {
        this.file.close();
    }

    /**
     * This method parses the cdr line into a GeneriRecord. This method reuses
     * the object to save the object (it does not create a new object each time)
     * 
     * @param cdrLine
     *            a String with the cdr to covert into a avro GenericRecord
     *            object
     */
    private void parseLine(String cdrLine) {

        Matcher m = pattern.matcher(cdrLine);
        if (m.matches()) {
            for (int i = 1; i <= m.groupCount(); i++) {
                String value = m.group(i);
                Field aux = this.parserList[i - 1];
                record.put(aux.name(),
                        parseValue(value, aux.schema().getType()));
            }
        }
    }

    /**
     * this method converts a string into the type define by parameter.
     * 
     * @param value
     *            the string with the value of the parameter
     * @param type
     *            the type of object that it converts the string
     * @return
     * @exception IllegalArgumentException
     *                if the type is not define
     * @exception ParserException
     *                if the string could not be into the type defined
     */
    private Object parseValue(String value, Type type) {

        if (type == Schema.Type.STRING) {
            return value;
        } else if (type == Schema.Type.INT) {
            return Integer.parseInt(value);
        } else {
            throw new IllegalArgumentException("");
        }
    }
}
