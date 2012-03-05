package es.tid.bdp.utils.data;

import es.tid.bdp.utils.parse.ParserAbstract;

public class BdpFileDescriptor {
    private boolean isCompressible;
    private boolean isReadable;
    private boolean isWritable;
    private ParserAbstract parser;

    public BdpFileDescriptor() {
        super();
    }

    /**
     * @return the isCompressible
     */
    public boolean isCompressible() {
        return isCompressible;
    }

    /**
     * @param isCompressible
     *            the isCompressible to set
     */
    public void setCompressible(boolean isCompressible) {
        this.isCompressible = isCompressible;
    }

    /**
     * @return the isReadable
     */
    public boolean isReadable() {
        return isReadable;
    }

    /**
     * @param isReadable
     *            the isReadable to set
     */
    public void setReadable(boolean isReadable) {
        this.isReadable = isReadable;
    }

    /**
     * @return the isWritable
     */
    public boolean isWritable() {
        return isWritable;
    }

    /**
     * @param isWritable
     *            the isWritable to set
     */
    public void setWritable(boolean isWritable) {
        this.isWritable = isWritable;
    }

    /**
     * @return the parser
     */
    public ParserAbstract getParser() {
        return parser;
    }

    /**
     * @param parser
     *            the parser to set
     */
    public void setParser(ParserAbstract parser) {
        this.parser = parser;
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#hashCode()
     */
    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + (isCompressible ? 1231 : 1237);
        result = prime * result + (isReadable ? 1231 : 1237);
        result = prime * result + (isWritable ? 1231 : 1237);
        return result;
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#equals(java.lang.Object)
     */
    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null)
            return false;
        if (getClass() != obj.getClass())
            return false;
        BdpFileDescriptor other = (BdpFileDescriptor) obj;
        if (isCompressible != other.isCompressible)
            return false;
        if (isReadable != other.isReadable)
            return false;
        if (isWritable != other.isWritable)
            return false;
        return true;
    }

}
