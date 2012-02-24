package es.tid.bdp.sftp.client;

import java.io.IOException;

import com.google.protobuf.Message;
import com.jcraft.jsch.JSchException;
import com.jcraft.jsch.SftpException;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.bdp.sftp.io.ParserAbstract;
import es.tid.bdp.sftp.io.ParserCdr;
import es.tid.bdp.utils.PropertiesPlaceHolder;

public class Prueba {

    public static void main(String[] arg) throws JSchException, SftpException,
            IOException, IllegalArgumentException, IllegalAccessException {

        PropertiesPlaceHolder
                .createInstance("src/main/resources/sftp-client.properties");
        ParserAbstract p = new ParserCdr();
        ProtobufWritable<Message> pb = p
        // .parseLine("33F430521676F4|2221436242|33F430521676F4|0442224173253|2|01/01/2010|02:00:01|2891|RMITERR");
                .parseLine("2721464427||2|02/04/2010|00:04:47|118-TELEFONIA MOVIL||MMS302");
        System.out.println(pb);
    }
}
