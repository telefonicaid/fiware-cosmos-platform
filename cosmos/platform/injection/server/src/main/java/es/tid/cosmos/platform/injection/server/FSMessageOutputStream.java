package es.tid.cosmos.platform.injection.server;

import java.io.IOException;
import java.io.OutputStream;
import java.util.Arrays;

import com.google.protobuf.Message;
import org.apache.hadoop.fs.FSDataOutputStream;

import es.tid.cosmos.base.data.MessageDescriptor;
import es.tid.cosmos.base.data.MessageGenerator;

/**
 *
 * @author dmicol
 */
public class FSMessageOutputStream extends FSDataOutputStream {
    private MessageDescriptor descriptor;
    private String buffer;
    
    public FSMessageOutputStream(OutputStream out, MessageDescriptor descriptor)
            throws IOException {
        super(out);
        this.buffer = "";
        this.descriptor = descriptor;
    }
    
    @Override
    public synchronized void write(int i) throws IOException {
        if (this.descriptor == null) {
            throw new IllegalStateException("Missing message descriptor");
        }
        final char c = (char)i;
        if (c == '\n' || c == '\r') {
            if (this.buffer.isEmpty()) {
                return;
            }
            final Message message = MessageGenerator.generate(this.descriptor,
                                                              this.buffer);
            message.writeTo(super.out);
            this.written = super.written;
            this.buffer = "";
        } else {
            this.buffer += c;
        }
    }

    @Override
    public void write(byte[] bytes) throws IOException {
        for (byte b : bytes) {
            this.write(b);
        }
    }
    
    @Override
    public synchronized void write(byte[] bytes, int i, int i1)
            throws IOException {
        this.write(Arrays.copyOfRange(bytes, i, i1));
    }
}
