package es.tid.ps.mobility.data;

import org.apache.hadoop.io.WritableComparable;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;

public class Cell implements WritableComparable<Cell> {

    private long idCell;
    private long geoLocationLevel1;
    private long geoLocationLevel2;
    private long geoLocationLevel3;
    private long geoLocationLevel4;
    private double longitude;
    private double latitude;

    public long getIdCell() {
        return idCell;
    }

    public void setIdCell(final long idCell) {
        this.idCell = idCell;
    }

    public long getGeoLocationLevel1() {
        return geoLocationLevel1;
    }

    public void setGeoLocationLevel1(final long geoLocationLevel1) {
        this.geoLocationLevel1 = geoLocationLevel1;
    }

    public long getGeoLocationLevel2() {
        return geoLocationLevel2;
    }

    public void setGeoLocationLevel2(final long geoLocationLevel2) {
        this.geoLocationLevel2 = geoLocationLevel2;
    }

    public long getGeoLocationLevel3() {
        return geoLocationLevel3;
    }

    public void setGeoLocationLevel3(final long geoLocationLevel3) {
        this.geoLocationLevel3 = geoLocationLevel3;
    }

    public long getGeoLocationLevel4() {
        return geoLocationLevel4;
    }

    public void setGeoLocationLevel4(final long geoLocationLevel4) {
        this.geoLocationLevel4 = geoLocationLevel4;
    }

    public double getLongitude() {
        return longitude;
    }

    public void setLongitude(final double longitude) {
        this.longitude = longitude;
    }

    public double getLatitude() {
        return latitude;
    }

    public void setLatitude(final double latitude) {
        this.latitude = latitude;
    }

    public Cell() {
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        final Cell cell = (Cell) o;

        if (geoLocationLevel1 != cell.geoLocationLevel1) {
            return false;
        }
        if (geoLocationLevel2 != cell.geoLocationLevel2) {
            return false;
        }
        if (geoLocationLevel3 != cell.geoLocationLevel3) {
            return false;
        }
        if (geoLocationLevel4 != cell.geoLocationLevel4) {
            return false;
        }
        if (idCell != cell.idCell) {
            return false;
        }
        if (Double.compare(cell.latitude, latitude) != 0) {
            return false;
        }
        if (Double.compare(cell.longitude, longitude) != 0) {
            return false;
        }

        return true;
    }

    @Override
    public int hashCode() {
        int result;
        long temp;
        result = (int) (idCell ^ (idCell >>> 32));
        result = 31 * result + (int) (geoLocationLevel1 ^ (geoLocationLevel1 >>> 32));
        result = 31 * result + (int) (geoLocationLevel2 ^ (geoLocationLevel2 >>> 32));
        result = 31 * result + (int) (geoLocationLevel3 ^ (geoLocationLevel3 >>> 32));
        result = 31 * result + (int) (geoLocationLevel4 ^ (geoLocationLevel4 >>> 32));
        temp = longitude != +0.0d ? Double.doubleToLongBits(longitude) : 0L;
        result = 31 * result + (int) (temp ^ (temp >>> 32));
        temp = latitude != +0.0d ? Double.doubleToLongBits(latitude) : 0L;
        result = 31 * result + (int) (temp ^ (temp >>> 32));
        return result;
    }


    @Override
    public String toString() {
        return "Cell{" + "idCell=" + idCell + ", geoLocationLevel1=" + geoLocationLevel1 + ", geoLocationLevel2=" + geoLocationLevel2 +
                ", geoLocationLevel3=" + geoLocationLevel3 + ", geoLocationLevel4=" + geoLocationLevel4 + ", longitude=" + longitude + ", latitude=" +
                latitude + '}';
    }


    @Override
    public void write(final DataOutput dataOutput) throws IOException {
        dataOutput.writeLong(this.idCell);
        dataOutput.writeLong(this.geoLocationLevel1);
        dataOutput.writeLong(this.geoLocationLevel2);
        dataOutput.writeLong(this.geoLocationLevel3);
        dataOutput.writeLong(this.geoLocationLevel4);
        dataOutput.writeDouble(this.latitude);
        dataOutput.writeDouble(this.longitude);
    }

    @Override
    public void readFields(final DataInput dataInput) throws IOException {
        this.idCell = dataInput.readLong();
        this.geoLocationLevel1 = dataInput.readLong();
        this.geoLocationLevel2 = dataInput.readLong();
        this.geoLocationLevel3 = dataInput.readLong();
        this.geoLocationLevel4 = dataInput.readLong();
        this.latitude = dataInput.readDouble();
        this.longitude = dataInput.readDouble();
    }

    @Override
    public int compareTo(final Cell cell) {
        if (cell.getIdCell() > this.getIdCell()) {
            return 1;
        } else if (cell.getIdCell() < this.getIdCell()) {
            return -1;
        } else {
            return 0;
        }
    }
}

