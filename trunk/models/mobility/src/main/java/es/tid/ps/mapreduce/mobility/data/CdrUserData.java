/**
 * 
 */
package es.tid.ps.mapreduce.mobility.data;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.NoSuchElementException;
import java.util.StringTokenizer;

/**
 * * Class that storages the information of each cdr file line. The format of
 * the line is <br>
 * Init_Cell|User_call|End_cell|User_recive|???|date_and_time|???|roaming_type
 * 
 * @author rgc
 */
public class CdrUserData {
    final public static String DELIMITER = "\\|";

    private Date dateCall;
    private String userId;

    private String initCellId;
    private String endCellId;
    private String roamingType;
    final private static SimpleDateFormat dateFormat = new SimpleDateFormat(
            "dd/MM/yyyyHH:mm:ss");

    /**
     * Constructor
     */
    public CdrUserData() {
        super();
    }

    /**
     * @return the userId
     */
    public String getUserId() {
        return userId;
    }

    /**
     * @param userId
     *            the userId to set
     */
    public void setUserId(String userId) {
        this.userId = userId;
    }

    /**
     * @return the dateCall
     */
    public Date getDateCall() {
        return dateCall;
    }

    /**
     * @param dateCall
     *            the dateCall to set
     */
    public void setDateCall(Date dateCall) {
        this.dateCall = dateCall;
    }

    /**
     * @return the initCellId
     */
    public String getInitCellId() {
        return initCellId;
    }

    /**
     * @param initCellId
     *            the initCellId to set
     */
    public void setInitCellId(String initCellId) {
        this.initCellId = initCellId;
    }

    /**
     * @return the endCellId
     */
    public String getEndCellId() {
        return endCellId;
    }

    /**
     * @param endCellId
     *            the endCellId to set
     */
    public void setEndCellId(String endCellId) {
        this.endCellId = endCellId;
    }

    /**
     * @return the roamingType
     */
    public String getRoamingType() {
        return roamingType;
    }

    /**
     * @param roamingType
     *            the roamingType to set
     */
    public void setRoamingType(String roamingType) {
        this.roamingType = roamingType;
    }

    /**
     * Method that parses a line a genererate a CdrUserData class
     * 
     * @param line
     *            string to parse
     * @return a CdrUserData class with the data line
     */
    static public CdrUserData parse(final String line) {
        try {
            CdrUserData cdrUserData = new CdrUserData();
            // se tokeniza la linea leida del archivo
            StringTokenizer stt = new StringTokenizer(line, DELIMITER);
            // Initial Cell
            cdrUserData.initCellId = stt.nextToken();
            // User caller
            cdrUserData.userId = stt.nextToken();
            // End cell
            cdrUserData.endCellId = stt.nextToken();
            // User reciver
            stt.nextToken();
            // ???
            stt.nextToken();
            // date and time
            cdrUserData.dateCall = dateFormat.parse(stt.nextToken()
                    + stt.nextToken());
            // ???
            stt.nextToken();
            // Roaming type
            cdrUserData.roamingType = stt.nextToken();
            return cdrUserData;
        } catch (NoSuchElementException nsee) {
            throw new IllegalArgumentException();
        } catch (ParseException e) {
            throw new IllegalArgumentException();
        }
    }
}
