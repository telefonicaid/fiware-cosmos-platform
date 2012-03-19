package es.tid.bdp.sftp.client;

import java.io.IOException;
import java.io.OutputStream;
import java.util.Vector;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

import com.jcraft.jsch.Channel;
import com.jcraft.jsch.ChannelSftp;
import com.jcraft.jsch.ChannelSftp.LsEntry;
import com.jcraft.jsch.JSch;
import com.jcraft.jsch.JSchException;
import com.jcraft.jsch.Session;
import com.jcraft.jsch.SftpException;

import es.tid.bdp.sftp.PropertiesPlaceHolder;
import es.tid.bdp.sftp.io.AvroCdrsOutStream;

public class SftpClientHandler {

    private final static String FS_DEFAULT_NAME = "fs.default.name";
    static final String SFTP_USER = "sftp.user";
    private static final String SFTP_HOST = "sftp.host";
    private static final String SFTP_PORT = "sftp.port";
    private static final String SFTP_CHANNEL = "sftp.channel";

    private static final String IO_INPUT_PATH = "io.input.path";
    private static final String IO_INPUT_REGEX = "io.input.regex";
    private static final String IO_OUPUT_PATH = "io.output.path";
    private static final String IO_OUPUT_REPLACE_REGEX = "io.output.replace.regex";
    private static final String IO_OUPUT_REPLACE_REPLACEMENT = "io.output.replace.replacement";
    private static final String IO_OUPUT_OVERWRITE = "io.output.overwrite";

    private String inputFile;
    private Path outputFile;
    private Boolean overwrite;
    private ChannelSftp fsSrc;
    private FileSystem hdfsDst;

    public void run() throws JSchException, IOException, SftpException {
        JSch jsch = new JSch();

        PropertiesPlaceHolder properties = PropertiesPlaceHolder.getInstance();

        this.overwrite = properties.getPropertyBool(IO_OUPUT_OVERWRITE);

        Session session = jsch.getSession(properties.getProperty(SFTP_USER),
                properties.getProperty(SFTP_HOST),
                properties.getPropertyInt(SFTP_PORT));
        session.setUserInfo(new PropUserInfo());
        session.connect();
        fsSrc = (ChannelSftp) session.openChannel(properties
                .getProperty(SFTP_CHANNEL));
        fsSrc.connect();

        Configuration confDst = new Configuration();
        confDst.set(FS_DEFAULT_NAME, properties.getProperty(FS_DEFAULT_NAME));
        hdfsDst = FileSystem.get(confDst);
        fsSrc.cd(properties.getProperty(IO_INPUT_PATH));

        @SuppressWarnings("unchecked")
        Vector<LsEntry> paths = fsSrc
                .ls(properties.getProperty(IO_INPUT_REGEX));
        for (int i = 0; i < paths.size(); i++) {
            LsEntry path = paths.elementAt(i);
            inputFile = path.getFilename();
            outputFile = new Path(properties.getProperty(IO_OUPUT_PATH),
                    inputFile.replace(properties
                            .getProperty(IO_OUPUT_REPLACE_REGEX), properties
                            .getProperty(IO_OUPUT_REPLACE_REPLACEMENT)));
            copy();
        }
        session.disconnect();
    }

    /**
     * This method copies a file from the cluster source to the cluster
     * destination
     * 
     * @throws IOException
     * @throws SftpException
     * @throws FileTransferException
     */
    private void copy() throws IOException, SftpException {
        System.out.println("copy from " + inputFile + " to " + outputFile);
        OutputStream out = null;

        if (!overwrite && hdfsDst.exists(outputFile)) {
            return;
        }
        try {
            out = new AvroCdrsOutStream(hdfsDst.create(outputFile));
            fsSrc.get(inputFile, out);
        } finally {
            if (out != null) {
                out.close();
            }
        }
    }
}
