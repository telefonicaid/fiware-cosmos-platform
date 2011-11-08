package es.tid.ps.kpicalculation.data;

/**
 * Enumeration of the fields in the input lines received by the module to be
 * stored in hive's table PAGE_VIEWS
 * 
 * @author javierb
 * 
 */
public enum PageViewInput {
    VISITOR_ID(0), URL(1), DATE_TIME(2), HTTP_STATUS(3), MIME_TYPE(4), USER_AGENT(
            5), METHOD(6);

    private Integer value;

    /**
     * Constructor
     * 
     * @param value
     *            the identifier of the file
     */
    PageViewInput(Integer value) {
        this.value = value;
    }

    /**
     * Get the value
     * 
     * @return the value
     */
    public Integer getValue() {
        return this.value;
    }
}
