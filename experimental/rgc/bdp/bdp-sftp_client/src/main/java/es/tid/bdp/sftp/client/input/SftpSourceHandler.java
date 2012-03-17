package es.tid.bdp.sftp.client.input;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

import javax.annotation.PostConstruct;

import com.jcraft.jsch.ChannelSftp;
import com.jcraft.jsch.ChannelSftp.LsEntry;
import com.jcraft.jsch.JSch;
import com.jcraft.jsch.Session;
import com.jcraft.jsch.SftpException;
import com.jcraft.jsch.UserInfo;

import es.tid.bdp.utils.PropertiesPlaceHolder;

/**
 * This class allows to connect to a sftp server, to open a authenticate
 * session, and to scan directories and to copy files to a stream destination
 * 
 * @author rgc
 * 
 */
public class SftpSourceHandler implements SourceHandler {

    static final String SFTP_USER = "sftp.user";
    private static final String SFTP_HOST = "sftp.host";
    private static final String SFTP_PORT = "sftp.port";
    private static final String SFTP_CHANNEL = "sftp.channel";
    private static final String SFTP_PASSWORD = "sftp.password";

    private ChannelSftp fsSrc;

    public SftpSourceHandler() {
        postConstructor();
    }

    /**
     * Postconstructor in where it reads the configuration and injects the
     * resources of the class, it is doing in this phase to facilitate the
     * generation the unit test
     */
    @PostConstruct
    private void postConstructor() {
        try {
            PropertiesPlaceHolder properties = PropertiesPlaceHolder
                    .getInstance();

            JSch jsch = new JSch();

            Session session = jsch.getSession(
                    properties.getProperty(SFTP_USER),
                    properties.getProperty(SFTP_HOST),
                    properties.getPropertyInt(SFTP_PORT));
            session.setUserInfo(new PropUserInfo(properties
                    .getProperty(SFTP_PASSWORD)));
            session.connect();
            fsSrc = (ChannelSftp) session.openChannel(properties
                    .getProperty(SFTP_CHANNEL));
            fsSrc.connect();
        } catch (Exception e) {
            // TODO: handle exception
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see
     * es.tid.bdp.sftp.client.input.SourceHandler#getFiles(java.lang.String,
     * java.lang.String)
     */
    @Override
    public List<String> getFiles(String path, String regex) {
        try {
            List<String> list = new ArrayList<String>();
            fsSrc.cd(path);

            @SuppressWarnings("unchecked")
            Vector<LsEntry> paths = fsSrc.ls(regex);

            for (int i = 0; i < paths.size(); i++) {
                LsEntry file = paths.elementAt(i);
                list.add(file.getFilename());
            }
            return list;
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see es.tid.bdp.sftp.client.input.SourceHandler#copy(java.lang.String,
     * java.io.OutputStream)
     */
    @Override
    public void copy(String inputFile, OutputStream outStream)
            throws IOException {
        try {
            fsSrc.get(inputFile, outStream);
        } catch (SftpException e) {
            throw new IOException(e);
        }
    }

    /**
     * Private class in where it is storage the password of the data
     * 
     * @author rgc
     * 
     */
    private class PropUserInfo implements UserInfo {

        private final String password;

        public PropUserInfo(String password) throws IOException {
            super();
            this.password = password;
        }

        public String getPassphrase() {
            return this.password;
        }

        public String getPassword() {
            return this.password;
        }

        public boolean promptPassword(String message) {
            return true;
        }

        public boolean promptPassphrase(String message) {
            return false;
        }

        public boolean promptYesNo(String message) {
            return true;
        }

        public void showMessage(String message) {
        }
    }
}
