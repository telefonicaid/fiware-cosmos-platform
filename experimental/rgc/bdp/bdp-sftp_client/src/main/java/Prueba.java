import java.io.IOException;

import es.tid.bdp.sftp.io.ParserCdr;
import es.tid.bdp.utils.PropertiesPlaceHolder;


public class Prueba {

    /**
     * @param args
     * @throws IOException 
     */
    public static void main(String[] args) throws IOException {
        // TODO Auto-generated method stub
        
        PropertiesPlaceHolder.createInstance("src/main/resources/sftp-client.properties");
        ParserCdr p = new ParserCdr();
        
        p.parseLine("2464708016||2|06/04/2010|13:09:39|188-TELCEL||MMS128");

    }

}
