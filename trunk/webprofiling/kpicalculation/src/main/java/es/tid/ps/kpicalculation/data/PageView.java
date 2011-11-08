package es.tid.ps.kpicalculation.data;

import java.net.MalformedURLException;

import org.apache.hadoop.conf.Configuration;
import org.apache.nutch.net.URLNormalizers;
import org.apache.nutch.net.urlnormalizer.basic.BasicURLNormalizer;
import org.apache.nutch.net.urlnormalizer.regex.RegexURLNormalizer;
import org.apache.nutch.util.NutchConfiguration;
import org.apache.xerces.util.URI;
import org.apache.xerces.util.URI.MalformedURIException;

/**
 * Class to parse input lines of CDR files to fit the required format of
 * PAGE_VIEWS table in hive for web profiling module
 * 
 * @author javierb
 * 
 */
/**
 * @author javierb
 *
 */
/**
 * @author javierb
 *
 */
/**
 * @author javierb
 * 
 */
public class PageView {
    final private static String DELIMITER = "\t";

    private String visitorId;
    private String protocol;
    private String fullUrl;
    private String urlDomain;
    private String urlPath;
    private String urlQuery;
    private String dateView;
    private String timeDay;
    private String userAgent;
    private String browser;
    private String device;
    private String operSys;
    private String method;
    private String status;

    /**
     * @return the visitorId
     */
    public String getVisitorId() {
        return visitorId;
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
        return protocol;
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
        return fullUrl;
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
        return urlDomain;
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
        return urlPath;
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
        return urlQuery;
    }

    /**
     * @param urlQuery
     *            the value to set
     */
    public void setUrlQuery(String urlQuery) {
        this.urlQuery = urlQuery;
    }

    /**
     * @return the dateView
     */
    public String getDateView() {
        return dateView;
    }

    /**
     * @param dateView
     *            the value to set
     */
    public void setDateView(String dateView) {
        this.dateView = dateView;
    }

    /**
     * @return the timeDay
     */
    public String getTimeDay() {
        return timeDay;
    }

    /**
     * @param timeDay
     *            the value to set
     */
    public void setTimeDay(String timeDay) {
        this.timeDay = timeDay;
    }

    /**
     * @return the userAgent
     */
    public String getUserAgent() {
        return userAgent;
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
        return browser;
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
        return device;
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
        return operSys;
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
        return method;
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
        return status;
    }

    /**
     * @param status
     *            the value to set
     */
    public void setStatus(String status) {
        this.status = status;
    }

    /**
     * Constructor method that parses a line and generates a PageView object
     * 
     * @param line
     *            string to parse
     * 
     */
    public PageView(String line) throws MalformedURLException,
            MalformedURIException {
        String[] listLine = line.trim().split(DELIMITER, -1);

        visitorId = listLine[PageViewInput.VISITOR_ID.getValue()];
        // The input url field is already stored normalized in this class
        fullUrl = normalize(listLine[PageViewInput.URL.getValue()]);

        // Protocol, domain, path and query parts of the url are obtained from
        // the fullurl field
        URI uri = new URI(fullUrl);
        protocol = uri.getScheme();
        urlDomain = uri.getHost();
        urlPath = uri.getPath();
        urlQuery = uri.getQueryString();

        // Date and time are obteined from the full date received in the cdr
        // line
        dateView = listLine[PageViewInput.DATE_TIME.getValue()];
        timeDay = listLine[PageViewInput.DATE_TIME.getValue()];

        // At this moment these four fields are the user agent.
        // TODO: It would be possible to process the user agent field for more
        // granularity
        userAgent = listLine[PageViewInput.USER_AGENT.getValue()];
        browser = listLine[PageViewInput.USER_AGENT.getValue()];
        device = listLine[PageViewInput.USER_AGENT.getValue()];
        operSys = listLine[PageViewInput.USER_AGENT.getValue()];

        method = listLine[PageViewInput.METHOD.getValue()];
        status = listLine[PageViewInput.HTTP_STATUS.getValue()];

        // TODO:Mime type missing in tables.This is pending;
    }

    /**
     * Method that receives an URL and applies two Nutch normalizers to it.
     * First is applied the BasicURLNormalizer which is in charge of standard
     * normalization processes, and after it the RegexURLNormalizer which is an
     * advanced normalizer whose functionalities are defined in a configuration
     * file.
     * 
     * @param inputUrl
     *            url to normalize
     * @return the normalized url
     * 
     */
    private String normalize(String inputUrl) throws MalformedURLException {
        String normalizedUrl;
        BasicURLNormalizer normalizer = new BasicURLNormalizer();
        Configuration conf = NutchConfiguration.create();
        normalizer.setConf(conf);

        normalizedUrl = normalizer.normalize(inputUrl,
                URLNormalizers.SCOPE_DEFAULT);

        // Advanced Nutch URL normalization based on regular expressions
        RegexURLNormalizer norm = new RegexURLNormalizer(conf);
        return norm.normalize(normalizedUrl, URLNormalizers.SCOPE_DEFAULT);
    }

    /**
     * Method that generates the string corresponding to the object. It will be
     * used to emit this data in the mapper.
     * 
     * @return the output line of the object
     * 
     */
    public String toString() {
        StringBuilder sBuilder = new StringBuilder();

        sBuilder.append(visitorId).append(DELIMITER);
        sBuilder.append(protocol).append(DELIMITER);
        sBuilder.append(fullUrl).append(DELIMITER);
        sBuilder.append(urlDomain).append(DELIMITER);
        sBuilder.append(urlPath).append(DELIMITER);
        sBuilder.append(urlQuery).append(DELIMITER);
        sBuilder.append(dateView).append(DELIMITER);
        sBuilder.append(timeDay).append(DELIMITER);
        sBuilder.append(userAgent).append(DELIMITER);
        sBuilder.append(browser).append(DELIMITER);
        sBuilder.append(device).append(DELIMITER);
        sBuilder.append(operSys).append(DELIMITER);
        sBuilder.append(method).append(DELIMITER);
        sBuilder.append(status);

        return sBuilder.toString();
    }
}
