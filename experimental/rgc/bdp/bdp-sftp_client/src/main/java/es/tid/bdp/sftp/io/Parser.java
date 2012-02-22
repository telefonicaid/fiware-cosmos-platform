package es.tid.bdp.sftp.io;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.pig.parser.ParserException;

import com.google.protobuf.DescriptorProtos.FileDescriptorProto;
import com.google.protobuf.DescriptorProtos.FileDescriptorSet;
import com.google.protobuf.Descriptors;
import com.google.protobuf.Descriptors.Descriptor;
import com.google.protobuf.Descriptors.DescriptorValidationException;
import com.google.protobuf.Descriptors.FieldDescriptor;
import com.google.protobuf.Descriptors.FileDescriptor;
import com.google.protobuf.DynamicMessage;
import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.bdp.utils.PropertiesPlaceHolder;

public class Parser {
    

    private static final String INPUT_ATTRS_REGEX = "input.attrs.regex";
    private static final String INPUT_ATTRS_DESC = "input.attrs.desc";
    private static final String IP_OUTPUT_DESCRIPTOR ="io.output.descriptor";

    private Pattern pattern;
    DynamicMessage.Builder builder;
    private FieldDescriptor[] parserList;

    public Parser() throws IOException {
        PropertiesPlaceHolder properties = PropertiesPlaceHolder.getInstance();

        this.pattern = Pattern.compile(properties
                .getProperty(INPUT_ATTRS_REGEX));

        String lineConfigurator = properties.getProperty(INPUT_ATTRS_DESC);

        // FileInputStream fin = new
        // FileInputStream("src/main/resources/protobuf/Test.desc");
        FileInputStream fin = new FileInputStream(
                properties.getProperty(IP_OUTPUT_DESCRIPTOR));

        load(fin, lineConfigurator);

    }

    /**
     * This method parses the cdr line into a GeneriRecord. This method reuses
     * the object to save the object (it does not create a new object each time)
     * 
     * @param cdrLine
     *            a String with the cdr to covert into a avro GenericRecord
     *            object
     * @return
     * @throws IllegalAccessException
     * @throws IllegalArgumentException
     */
    public ProtobufWritable<Message> parseLine(String cdrLine)
            throws IllegalArgumentException, IllegalAccessException {
        // System.out.println(cdrLine);
        Matcher m = pattern.matcher(cdrLine);
        DynamicMessage.Builder cloneBuilder = builder.clone();
        if (m.matches()) {
            for (int i = 1; i <= m.groupCount(); i++) {
                String value = m.group(i);
                FieldDescriptor aux = this.parserList[i - 1];
                cloneBuilder.setField(aux, parseValue(value, aux.getType()));
            }
        }

        ProtobufWritable<Message> protobufWritable = ProtobufWritable
                .newInstance(Message.class);

        protobufWritable.set(cloneBuilder.build());
        return protobufWritable;
    }

    /**
     * this method converts a string into the type define by parameter.
     * 
     * @param value
     *            the string with the value of the parameter
     * 
     * @param type
     *            the type of object that it converts the string
     * 
     * @return
     * 
     * @exception IllegalArgumentException
     *                if the type is not define
     * 
     * @exception ParserException
     *                if the string could not be into the type defined
     */
    private Object parseValue(String value,
            com.google.protobuf.Descriptors.FieldDescriptor.Type type) {

        return value;
    }

    /**
     * 
     * @param inputStream
     * @param lineConfigurator
     */
    public void load(InputStream inputStream, String lineConfigurator) {
        try {
            FileDescriptorSet descriptorSet = FileDescriptorSet
                    .parseFrom(inputStream);
            List<FileDescriptorProto> fileList = descriptorSet.getFileList();
            FileDescriptorProto fileDescriptorProto = fileList.get(0);
            List<FileDescriptor> loaded = new ArrayList<Descriptors.FileDescriptor>();
            FileDescriptor[] ar = new FileDescriptor[loaded.size()];
            loaded.toArray(ar);
            FileDescriptor fileDescriptor = FileDescriptor.buildFrom(
                    fileDescriptorProto, ar);

            Descriptor descriptor = fileDescriptor.getMessageTypes().get(0);

            builder = DynamicMessage.getDefaultInstance(descriptor)
                    .newBuilderForType();

            descriptor.getFields();

            Map<String, FieldDescriptor> map = new HashMap<String, Descriptors.FieldDescriptor>();
            for (FieldDescriptor fieldDescriptor : descriptor.getFields()) {
                System.out.println(fieldDescriptor.getName());
                map.put(fieldDescriptor.getName(), fieldDescriptor);
            }

            String[] array = lineConfigurator.split("\\|");
            this.parserList = new FieldDescriptor[array.length];

            for (int i = 0; i < array.length; i++) {
                System.out.println(array[i]);
                this.parserList[i] = map.get(array[i]);
            }

        } catch (IOException e) {
        } catch (DescriptorValidationException e) {
        }
    }

}
