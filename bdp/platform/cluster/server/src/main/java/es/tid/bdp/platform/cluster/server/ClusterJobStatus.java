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

public enum ClusterJobStatus implements org.apache.thrift.TEnum {
  NOT_STARTED(1),
  RUNNING(2),
  SUCCESSFUL(3),
  FAILED(4);

  private final int value;

  private ClusterJobStatus(int value) {
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
  public static ClusterJobStatus findByValue(int value) { 
    switch (value) {
      case 1:
        return NOT_STARTED;
      case 2:
        return RUNNING;
      case 3:
        return SUCCESSFUL;
      case 4:
        return FAILED;
      default:
        return null;
    }
  }
}