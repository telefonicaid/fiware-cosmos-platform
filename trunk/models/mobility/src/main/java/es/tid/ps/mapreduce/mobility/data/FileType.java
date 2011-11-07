package es.tid.ps.mapreduce.mobility.data;

/**
 * Enum that define the type of files that process into the mobility, and
 * identifier each type of file with a int
 * 
 * @author rgc
 */
public enum FileType {
    FILE_CDRS(0), FILE_USERS(1);
    private Integer value;

    /**
     * Constructor
     * 
     * @param value
     *            the identifier of the file
     */
    FileType(Integer value) {
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
