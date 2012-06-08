package es.tid.cosmos.mobility.data;

import java.nio.charset.Charset;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.BinaryConverter;
import com.twitter.elephantbird.mapreduce.io.ProtobufConverter;
import org.apache.commons.lang.ArrayUtils;

/**
 *
 * @author ximo
 */
public class MobilityConverter<M extends Message> implements BinaryConverter<M> {
    private static final Charset UTF8 = Charset.forName("UTF-8");
    private Class clazz;

    public MobilityConverter(Class<M> clazz) {
        this.clazz = clazz;
    }

    @Override
    public byte[] toBytes(M message) {
        byte[] classBytes = this.clazz.getName().getBytes(UTF8);
        if (classBytes.length > Byte.MAX_VALUE) {
            throw new IllegalArgumentException("Class name is too long: "
                    + this.clazz.getName());
        }
        byte[] classSerialization = ArrayUtils.addAll(
                new byte[]{(byte)classBytes.length}, classBytes);
        ProtobufConverter converter = ProtobufConverter.newInstance(this.clazz);
        return ArrayUtils.addAll(classSerialization, converter.toBytes(message));
    }

    @Override
    public M fromBytes(byte[] messageBuffer) {
        byte classNameLength = messageBuffer[0];
        byte[] classNameBytes = ArrayUtils.subarray(messageBuffer, 1,
                                                    1 + classNameLength);
        String className = new String(classNameBytes, UTF8);
        try {
            this.clazz = Class.forName(className);
        } catch (ClassNotFoundException ex) {
            throw new IllegalArgumentException("Byte stream does not have a "
                    + "valid class identifier", ex);
        }
        ProtobufConverter<M> converter = ProtobufConverter.newInstance(this.clazz);
        return converter.fromBytes(ArrayUtils.subarray(messageBuffer,
                1 + classNameLength, messageBuffer.length));
    }
}
