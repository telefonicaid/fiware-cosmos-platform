/**
 * 
 */
package es.tid.mapreduce.mobility2.data;

import java.util.StringTokenizer;

/**
 * Class that storages the information of each cell. It has a static method to
 * parse the data from file
 * 
 * @author rgc
 */
public class CellData {
    final private static String DELIMITER = "\\|";
    final private static char DOT = '.';
    final private static char COMMA = ',';

    private String cellId;
    private String bts;
    private String state;
    private String country;
    private Double latitude;
    private Double longitude;

    public CellData(){
    }
    
    public CellData(String cellId, String bts, String state, String country,
            Double latitude, Double longitude) {
        super();
        this.cellId = cellId;
        this.bts = bts;
        this.state = state;
        this.country = country;
        this.latitude = latitude;
        this.longitude = longitude;
    }

    /**
     * @return the longitude
     */
    public Double getLongitude() {
        return longitude;
    }

    /**
     * @param longitude
     *            the longitude to set
     */
    public void setLongitude(Double longitude) {
        this.longitude = longitude;
    }

    /**
     * @return the latitude
     */
    public double getLatitude() {
        return latitude;
    }

    /**
     * @param latitude
     *            the latitude to set
     */
    public void setLatitude(Double latitude) {
        this.latitude = latitude;
    }

    /**
     * @return the country
     */
    public String getCountry() {
        return country;
    }

    /**
     * @param country
     *            the country to set
     */
    public void setCountry(String country) {
        this.country = country;
    }

    /**
     * @return the state
     */
    public String getState() {
        return state;
    }

    /**
     * @param state
     *            the state to set
     */
    public void setState(String state) {
        this.state = state;
    }

    /**
     * @return the bts
     */
    public String getBts() {
        return bts;
    }

    /**
     * @param bts
     *            the bts to set
     */
    public void setBts(String bts) {
        this.bts = bts;
    }

    /**
     * @return the cellId
     */
    public String getCellId() {
        return cellId;
    }

    /**
     * @param cellId
     *            the cellId to set
     */
    public void setCellId(String cellId) {
        this.cellId = cellId;
    }

    /**
     * Method that parses a line a genererate a CdrUserData class.
     * 3E885A5|13421|1000|1|-102,26605|21,8945861
     * 
     * @param line
     *            string to parse
     * @return a CdrUserData class with the data line
     */
    static public CellData parse(final String line) {
        CellData cellData = new CellData();
        // se tokeniza la linea leida del archivo
        StringTokenizer stt = new StringTokenizer(line, DELIMITER);
        // cellId
        cellData.cellId = stt.nextToken();
        // bts
        cellData.bts = stt.nextToken();
        // state
        cellData.state = stt.nextToken();
        // country
        cellData.country = stt.nextToken();
        // latitude
        cellData.setLatitude(Double.parseDouble(stt.nextToken().replace(COMMA,
                DOT)));
        // country
        cellData.setLongitude(Double.parseDouble(stt.nextToken().replace(COMMA,
                DOT)));

        return cellData;
    }  
}
