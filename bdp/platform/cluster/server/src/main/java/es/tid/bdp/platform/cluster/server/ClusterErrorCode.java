/**
 * Autogenerated by Thrift Compiler (0.8.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
package es.tid.bdp.platform.cluster.server;


import java.util.Map;
import java.util.HashMap;
import org.apache.thrift.TEnum;

public enum ClusterErrorCode implements org.apache.thrift.TEnum {
  FILE_COPY_FAILED(1),
  RUN_JOB_FAILED(2),
  INVALID_JOB_ID(3);

  private final int value;

  private ClusterErrorCode(int value) {
    this.value = value;
  }

  /**
   * Get the integer value of this enum value, as defined in the Thrift IDL.
   */
  public int getValue() {
    return value;
  }

  /**
   * Find a the enum type by its integer value, as defined in the Thrift IDL.
   * @return null if the value is not found.
   */
  public static ClusterErrorCode findByValue(int value) { 
    switch (value) {
      case 1:
        return FILE_COPY_FAILED;
      case 2:
        return RUN_JOB_FAILED;
      case 3:
        return INVALID_JOB_ID;
      default:
        return null;
    }
  }
}