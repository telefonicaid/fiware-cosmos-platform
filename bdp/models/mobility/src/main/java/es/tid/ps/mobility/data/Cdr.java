package es.tid.ps.mobility.data;

import java.util.Date;

public class Cdr {
    private long idNode;
    private long idCell;
    private Date date;

    public long getNode() {
        return idNode;
    }

    public void setNode(final long node) {
        this.idNode = node;
    }

    public long getIdCell() {
        return idCell;
    }

    public void setIdCell(final long idCell) {
        this.idCell = idCell;
    }

    public Date getDate() {
        return date;
    }

    public void setDate(final Date date) {
        this.date = date;
    }

    public Cdr() {
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((date == null) ? 0 : date.hashCode());
        result = prime * result + (int) (idCell ^ (idCell >>> 32));
        result = prime * result + (int) (idNode ^ (idNode >>> 32));
        return result;
    }

    @Override
    public boolean equals(final Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final Cdr other = (Cdr) obj;
        if (date == null) {
            if (other.date != null) {
                return false;
            }
        } else if (!date.equals(other.date)) {
            return false;
        }
        if (idCell != other.idCell) {
            return false;
        }
        if (idNode != other.idNode) {
            return false;
        }
        return true;
    }

    @Override
    public String toString() {
        return "Cdr{" + "idNode=" + idNode + ", idCell=" + idCell + ", date=" + date + '}';
    }
}
