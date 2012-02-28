package es.tid.bdp.utils.data;

public class BdpFileDescriptor {
    private boolean isSerializable;
    private boolean isReadable;
    private boolean isWritable;
    private BdpCompresion compresion;

    public BdpFileDescriptor() {
        super();
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
     * @param isWritable
     *            the isWritable to set
     */
    public void setWritable(boolean isWritable) {
        this.isWritable = isWritable;
    }

    /**
     * /**
     * 
     * @return the isSerializable
     */
    public boolean isSerializable() {
        return isSerializable;
    }

    /**
     * @param isSerializable
     *            the isSerializable to set
     */
    public void setSerializable(boolean isSerializable) {
        this.isSerializable = isSerializable;
    }

    /**
     * @return the compresion
     */
    public BdpCompresion getCompresion() {
        return compresion;
    }

    /**
     * @param compresion
     *            the compresion to set
     */
    public void setCompresion(BdpCompresion compresion) {
        this.compresion = compresion;
    }

    public static final class BdpCompresion {
        private final String className;
        private final String pattern;
        private final String attr;

        private BdpCompresion(String className, String pattern, String attr) {
            this.className = className;
            this.pattern = pattern;
            this.attr = attr;
        }

        public static BdpCompresion createInstance(String className,
                String pattern, String attr) {
            return new BdpCompresion(className, pattern, attr);
        }

        /**
         * @return the className
         */
        public String getClassName() {
            return className;
        }

        /**
         * @return the pattern
         */
        public String getPattern() {
            return pattern;
        }

        /**
         * @return the attr
         */
        public String getAttr() {
            return attr;
        }
    }
}
