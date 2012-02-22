package es.tid.bdp.sftp.client;

import java.io.File;
import java.io.IOException;
import java.util.List;

import org.apache.hadoop.fs.Path;

import com.jcraft.jsch.JSchException;
import com.jcraft.jsch.SftpException;

public class ConnectionHandler {
    
    private String path;
    private String regex;

    public ConnectionHandler(){
        this.path = path;
        this.regex = regex;
    }
    
    
    public void run() throws JSchException, IOException, SftpException{
        SftpSourceHandler client = new SftpSourceHandler();
        DestinationHandler dest = new DestinationHandler();

        List<String> list = client.getFiles(path, regex);
                
        for (String inputPath : list) {
            
            String ouputPath = convertPath(inputPath);
            
            client.copy(inputPath, dest.getOutputStream(ouputPath));
        }
    
    }

    private String convertPath(String inputPath) {
        /*
        inputFile = path.getFilename();
        
        File file = new File(inputPath);
        File outputFile = new File(file.getAbsolutePath(),
                file.getName().replace(properties
                        .getProperty(IO_OUPUT_REPLACE_REGEX), properties
                        .getProperty(IO_OUPUT_REPLACE_REPLACEMENT)));
        */
        return null;
    }

}
