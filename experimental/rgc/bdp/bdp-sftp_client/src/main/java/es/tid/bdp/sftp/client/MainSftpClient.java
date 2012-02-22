package es.tid.bdp.sftp.client;

import java.io.IOException;

import com.jcraft.jsch.JSchException;
import com.jcraft.jsch.SftpException;

import es.tid.bdp.utils.PropertiesPlaceHolder;

public class MainSftpClient {

    private static final String CONFIG_FILE = "-cf";

    /**
     * 
     * @param arg
     * @throws JSchException
     * @throws SftpException
     * @throws IOException
     */
    public static void main(String[] arg) throws JSchException, SftpException,
            IOException {
        // If there are arguments, it uses to load the properties file
        for (int i = 0; i < arg.length; i++) {
            if (CONFIG_FILE.equals(arg[i])){
                PropertiesPlaceHolder.createInstance(arg[i+1]);
                break; 
            }
        }
        
        // Run the process
        ConnectionHandler sftp = new ConnectionHandler();
        sftp.run();
    }    
}