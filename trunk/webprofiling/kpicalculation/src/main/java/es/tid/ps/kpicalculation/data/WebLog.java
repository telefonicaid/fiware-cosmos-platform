package es.tid.ps.kpicalculation.data;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import java.util.Calendar;
import java.util.NoSuchElementException;
import java.util.StringTokenizer;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.WritableComparable;
import org.apache.nutch.util.URLUtil;
import org.apache.xerces.util.URI;

import es.tid.ps.kpicalculation.utils.KpiCalculationDateFormatter;
import es.tid.ps.kpicalculation.utils.KpiCalculationNormalizer;

/**
 * Class to parse input lines of CDR files to fit the required format of a
 * WebLog inside webprofiling module, to use it to calculate different kpis
 * 
 * @author javierb
 */
public class WebLog implements WritableComparable<WebLog> {
    protected static final String DELIMITER = "\t";

    public String mainKey;
    public String secondaryKey;

    private StringBuilder sBuilder;

    public String visitorId;
    public String protocol;
    public String fullUrl;
    public String urlDomain;
    public String urlPath;
    public String urlQuery;
    public Calendar date;
    public String userAgent;
    public String browser;
    public String device;
    public String operSys;
    public String method;
    public String status;
    private Text text;

    /**
     * @return the visitorId
     */
    public String getVisitorId() {
        return this.visitorId;
    }

    /**
     * @param visitorId
     *            the value to set
     */
    public void setVisitorId(String visitorId) {
        this.visitorId = visitorId;
    }

    /**
     * @return the protocol
     */
    public String getProtocol() {
        return this.protocol;
    }

    /**
     * @param protocol
     *            the value to set
     */
    public void setProtocol(String protocol) {
        this.protocol = protocol;
    }

    /**
     * @return the fullUrl
     */
    public String getFullUrl() {
        return this.fullUrl;
    }

    /**
     * @param fullUrl
     *            the value to set
     */
    public void setFullUrl(String fullUrl) {
        this.fullUrl = fullUrl;
    }

    /**
     * @return the urlDomain
     */
    public String getUrlDomain() {
        return this.urlDomain;
    }

    /**
     * @param urlDomain
     *            the value to set
     */
    public void setUrlDomain(String urlDomain) {
        this.urlDomain = urlDomain;
    }

    /**
     * @return the urlPath
     */
    public String getUrlPath() {
        return this.urlPath;
    }

    /**
     * @param urlPath
     *            the value to set
     */
    public void setUrlPath(String urlPath) {
        this.urlPath = urlPath;
    }

    /**
     * @return the urlQuery
     */
    public String getUrlQuery() {
        return this.urlQuery;
    }

    /**
     * @param urlQuery
     *            the value to set
     */
    public void setUrlQuery(String urlQuery) {
        this.urlQuery = urlQuery;
    }

    /**
     * @return the date
     */
    public Calendar getDate() {
        return this.date;
    }

    /**
     * @param date
     *            the value to set
     */
    public void setDate(Calendar date) {
        this.date = date;
    }

    /**
     * @return the userAgent
     */
    public String getUserAgent() {
        return this.userAgent;
    }

    /**
     * @param userAgent
     *            the value to set
     */
    public void setUserAgent(String userAgent) {
        this.userAgent = userAgent;
    }

    /**
     * @return the browser
     */
    public String getBrowser() {
        return this.browser;
    }

    /**
     * @param browser
     *            the value to set
     */
    public void setBrowser(String browser) {
        this.browser = browser;
    }

    /**
     * @return the device
     */
    public String getDevice() {
        return this.device;
    }

    /**
     * @param device
     *            the value to set
     */
    public void setDevice(String device) {
        this.device = device;
    }

    /**
     * @return the operSys
     */
    public String getOperSys() {
        return this.operSys;
    }

    /**
     * @param operSys
     *            the value to set
     */
    public void setOperSys(String operSys) {
        this.operSys = operSys;
    }

    /**
     * @return the method
     */
    public String getMethod() {
        return this.method;
    }

    /**
     * @param method
     *            the value to set
     */
    public void setMethod(String method) {
        this.method = method;
    }

    /**
     * @return the status
     */
    public String getStatus() {
        return this.status;
    }

    /**
     * @param status
     *            the value to set
     */
    public void setStatus(String status) {
        this.status = status;
    }

    /**
     * @return the text
     */
    public Text getText() {
        return this.text;
    }

    /**
     * @param text
     *            the value to set
     */
    public void setText(Text text) {
        this.text = text;
    }

    /**
     * Constructor method that generates a WebLog object. Needed initializations
     * are done in this method.
     */
    public WebLog() {
        this.text = new Text();
        this.sBuilder = new StringBuilder();
        this.date = Calendar.getInstance();
        KpiCalculationNormalizer.init();
        KpiCalculationDateFormatter.init(WebLog.DELIMITER);
    }

    /**
     * Method that parses a web log line and sets the properties of the class
     * 
     * @param line
     *            string to parse
     */
    public void set(String line) throws IllegalAccessException,
            NoSuchFieldException {
        try {
            StringTokenizer stt = new StringTokenizer(line, DELIMITER);

            // The id of the user generating the log
            this.visitorId = stt.nextToken();

            // The input url field is already stored normalized in this class
            this.fullUrl = KpiCalculationNormalizer.normalize(stt.nextToken());

            // Protocol, domain, path and query parts of the url are obtained
            // from the fullurl field URI
            URI uri = new URI(this.fullUrl);
            this.protocol = uri.getScheme();
            this.urlDomain = URLUtil.getDomainName(this.fullUrl);
            this.urlPath = uri.getPath();
            this.urlQuery = uri.getQueryString();

            // Date and time are obteined from the full date received in the cdr
            // line
            this.date = KpiCalculationDateFormatter.getValue(stt.nextToken());

            // Http status code
            this.status = stt.nextToken();

            // Mime currently not used
            // TODO:Mime type missing in tables.This is pending;
            stt.nextToken();

            // User agent used in the navigation
            String clientInfo = stt.nextToken();
            this.userAgent = clientInfo;
            this.browser = clientInfo;
            this.device = clientInfo;
            this.operSys = clientInfo;

            // method of the request
            this.method = stt.nextToken();

            if (stt.hasMoreTokens()) {
                throw new KpiCalculationDataException(
                        "The line has too many fields",
                        KpiCalculationCounter.WRONG_LINE_FORMAT);
            }

            this.initText();
        } catch (Exception ex) {
            throw new KpiCalculationDataException("The URL is malformed",
                    KpiCalculationCounter.MALFORMED_URL);
        }
    }

    @Override
    public String toString() {
        return this.sBuilder.toString();
    }

    /**
     * Method that sets the properties of the class from a Text object
     * 
     * @param line
     *            string to parse
     */
    public void set(Text txt) throws IllegalAccessException,
            NoSuchFieldException {
        this.text = txt;

        StringTokenizer stt = new StringTokenizer(txt.toString(), DELIMITER);
        try {
            this.visitorId = stt.nextToken();
            this.protocol = stt.nextToken();
            this.fullUrl = stt.nextToken();
            this.urlDomain = stt.nextToken();
            this.urlPath = stt.nextToken();
            this.urlQuery = stt.nextToken();
            this.date.set(Calendar.DAY_OF_MONTH,
                    Integer.parseInt(stt.nextToken()));
            //Calendar months values are from  0:January to 11:December
            this.date.set(Calendar.MONTH, Integer.parseInt(stt.nextToken()) - 1);
            this.date.set(Calendar.YEAR, Integer.parseInt(stt.nextToken()));
            this.date.set(Calendar.HOUR_OF_DAY,
                    Integer.parseInt(stt.nextToken()));
            this.date.set(Calendar.MINUTE, Integer.parseInt(stt.nextToken()));
            this.userAgent = stt.nextToken();
            this.browser = stt.nextToken();
            this.device = stt.nextToken();
            this.operSys = stt.nextToken();
            this.method = stt.nextToken();
            this.status = stt.nextToken();
        } catch (NoSuchElementException e) {
            throw new KpiCalculationDataException("The URL is malformed",
                    KpiCalculationCounter.MALFORMED_URL);
        }
    }

    /**
     * Method that generates the string corresponding to the object. It will be
     * used to emit this data in the mapper.
     * 
     * @return the output line of the object
     * 
     */
    private void initText() {
        this.sBuilder.delete(0, sBuilder.length());

        this.sBuilder.append(visitorId).append(DELIMITER);
        this.sBuilder.append(protocol).append(DELIMITER);
        this.sBuilder.append(fullUrl).append(DELIMITER);
        this.sBuilder.append(urlDomain).append(DELIMITER);
        this.sBuilder.append(urlPath).append(DELIMITER);
        this.sBuilder.append(urlQuery).append(DELIMITER);
        this.sBuilder.append(date.get(Calendar.DAY_OF_MONTH)).append(DELIMITER);
        this.sBuilder.append(date.get(Calendar.MONTH)).append(DELIMITER);
        this.sBuilder.append(date.get(Calendar.YEAR)).append(DELIMITER);
        this.sBuilder.append(date.get(Calendar.HOUR_OF_DAY)).append(DELIMITER);
        this.sBuilder.append(date.get(Calendar.MINUTE)).append(DELIMITER);
        this.sBuilder.append(userAgent).append(DELIMITER);
        this.sBuilder.append(browser).append(DELIMITER);
        this.sBuilder.append(device).append(DELIMITER);
        this.sBuilder.append(operSys).append(DELIMITER);
        this.sBuilder.append(method).append(DELIMITER);
        this.sBuilder.append(status);

        this.text.set(this.sBuilder.toString());
    }

    @Override
    public void readFields(DataInput in) throws IOException {
        this.text.set(in.readUTF());
    }

    @Override
    public void write(DataOutput out) throws IOException {
        out.writeUTF(this.text.toString());
    }

    @Override
    public int compareTo(WebLog pv) {
        return this.text.toString().compareTo(pv.text.toString());
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#equals(java.lang.Object)
     */
    @Override
    public boolean equals(Object o) {
        boolean result = false;
        if (o instanceof WebLog) {
            WebLog pv = (WebLog) o;
            result = this.toString().equals(pv.toString());
        }
        return result;
    }

    @Override
    public int hashCode() {
        int hash = 3;
        hash = 97 * hash + (this.mainKey != null ? this.mainKey.hashCode() : 0);
        hash = 97 * hash + (this.secondaryKey != null ? this.secondaryKey.hashCode() : 0);
        hash = 97 * hash + (this.visitorId != null ? this.visitorId.hashCode() : 0);
        hash = 97 * hash + (this.protocol != null ? this.protocol.hashCode() : 0);
        hash = 97 * hash + (this.fullUrl != null ? this.fullUrl.hashCode() : 0);
        hash = 97 * hash + (this.urlDomain != null ? this.urlDomain.hashCode() : 0);
        hash = 97 * hash + (this.urlPath != null ? this.urlPath.hashCode() : 0);
        hash = 97 * hash + (this.urlQuery != null ? this.urlQuery.hashCode() : 0);
        hash = 97 * hash + (this.userAgent != null ? this.userAgent.hashCode() : 0);
        hash = 97 * hash + (this.browser != null ? this.browser.hashCode() : 0);
        hash = 97 * hash + (this.device != null ? this.device.hashCode() : 0);
        hash = 97 * hash + (this.operSys != null ? this.operSys.hashCode() : 0);
        hash = 97 * hash + (this.method != null ? this.method.hashCode() : 0);
        hash = 97 * hash + (this.status != null ? this.status.hashCode() : 0);
        return hash;
    }
}
