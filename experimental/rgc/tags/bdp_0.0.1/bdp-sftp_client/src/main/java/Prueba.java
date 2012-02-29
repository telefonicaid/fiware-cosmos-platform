import java.io.IOException;

import es.tid.bdp.utils.BuilderDdpFileDescriptorXml;
import es.tid.bdp.utils.PropertiesPlaceHolder;


public class Prueba {

    /**
     * @param args
     * @throws IOException 
     */
    public static void main(String[] args) throws IOException {
        // TODO Auto-generated method stub
        
        PropertiesPlaceHolder p =PropertiesPlaceHolder.createInstance("/Users/rgc/Documents/workspace/bdp/bdp-sftp_server/src/main/resources/sftp-server.properties");

        BuilderDdpFileDescriptorXml g = new BuilderDdpFileDescriptorXml(p);
        g.build("hdfs", "/erawr/rrr");
    }



}
