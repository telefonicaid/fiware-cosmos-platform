package es.tid.bdp.sftp.client;

import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.util.List;

import com.jcraft.jsch.JSchException;
import com.jcraft.jsch.SftpException;

import es.tid.bdp.utils.PropertiesPlaceHolder;

public class ConnectionHandler {
    

    private static final String IO_INPUT_PATH = "io.input.path";
    private static final String IO_INPUT_REGEX = "io.input.regex";
    private static final String IO_OUPUT_PATH = "io.output.path";
    private static final String IO_OUPUT_REPLACE_REGEX = "io.output.replace.regex";
    private static final String IO_OUPUT_REPLACE_REPLACEMENT = "io.output.replace.replacement";
    
    
    public void run() throws JSchException, IOException, SftpException{
        PropertiesPlaceHolder properties = PropertiesPlaceHolder.getInstance();

        SftpSourceHandler client = new SftpSourceHandler();
        DestinationHandler dest = new DestinationHandler();
        
        String path = properties.getProperty(IO_INPUT_PATH);
        String regex = properties.getProperty(IO_INPUT_REGEX);

        List<String> list = client.getFiles(path, regex);
                
        for (String inputPath : list) {
             String ouputPath = convertPath(inputPath);
            OutputStream output = dest.getOutputStream(ouputPath);
            client.copy(inputPath, output);
        }
    
    }

    private String convertPath(String inputPath) throws  IOException {
        PropertiesPlaceHolder properties = PropertiesPlaceHolder.getInstance();
        
        File file = new File(inputPath);
        File outputFile = new File(properties.getProperty(IO_OUPUT_PATH),
                file.getName().replace(properties
                        .getProperty(IO_OUPUT_REPLACE_REGEX), properties
                        .getProperty(IO_OUPUT_REPLACE_REPLACEMENT)));
        
        return outputFile.getAbsolutePath();
    }

}
