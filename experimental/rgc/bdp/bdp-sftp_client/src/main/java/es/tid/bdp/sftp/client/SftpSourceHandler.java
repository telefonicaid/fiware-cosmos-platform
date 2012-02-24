package es.tid.bdp.sftp.client;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

import com.jcraft.jsch.ChannelSftp;
import com.jcraft.jsch.ChannelSftp.LsEntry;
import com.jcraft.jsch.JSch;
import com.jcraft.jsch.JSchException;
import com.jcraft.jsch.Session;
import com.jcraft.jsch.SftpException;

import es.tid.bdp.utils.PropertiesPlaceHolder;

public class SftpSourceHandler {

    static final String SFTP_USER = "sftp.user";
    private static final String SFTP_HOST = "sftp.host";
    private static final String SFTP_PORT = "sftp.port";
    private static final String SFTP_CHANNEL = "sftp.channel";

    private ChannelSftp fsSrc;

    public SftpSourceHandler() throws JSchException, IOException {
        PropertiesPlaceHolder properties = PropertiesPlaceHolder.getInstance();

        JSch jsch = new JSch();

        Session session = jsch.getSession(properties.getProperty(SFTP_USER),
                properties.getProperty(SFTP_HOST),
                properties.getPropertyInt(SFTP_PORT));
        session.setUserInfo(new PropUserInfo());
        session.connect();
        fsSrc = (ChannelSftp) session.openChannel(properties
                .getProperty(SFTP_CHANNEL));
        fsSrc.connect();
    }

    public List<String> getFiles(String path, String regex)
            throws JSchException, IOException, SftpException {

        List<String> list = new ArrayList<String>();
        fsSrc.cd(path);

        @SuppressWarnings("unchecked")
        Vector<LsEntry> paths = fsSrc.ls(regex);

        for (int i = 0; i < paths.size(); i++) {
            LsEntry file = paths.elementAt(i);
            list.add(file.getFilename());
        }
        return list;
    }

    /**
     * This method copies a file from the cluster source to the cluster
     * destination
     * 
     * @throws IOException
     * @throws SftpException
     * @throws FileTransferException
     */
    public void copy(String inputFile, OutputStream outStream)
            throws IOException, SftpException {        
        fsSrc.get(inputFile, outStream);
    }
}
