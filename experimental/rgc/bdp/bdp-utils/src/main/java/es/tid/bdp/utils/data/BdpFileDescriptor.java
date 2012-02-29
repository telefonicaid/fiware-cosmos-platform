package es.tid.bdp.utils.data;

import es.tid.bdp.utils.parse.ParserAbstract;

public class BdpFileDescriptor {
    private boolean isCompressible;
    private boolean isReadable;
    private boolean isWritable;
    private  ParserAbstract parser;

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
     * @param isCompressible the isCompressible to set
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
     * @param isReadable the isReadable to set
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
     * @param isWritable the isWritable to set
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
     * @param parser the parser to set
     */
    public void setParser(ParserAbstract parser) {
        this.parser = parser;
    }


}
