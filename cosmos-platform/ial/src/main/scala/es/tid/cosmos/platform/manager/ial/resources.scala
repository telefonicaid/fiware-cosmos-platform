package es.tid.cosmos.platform.manager.ial

import java.util.UUID

/**
 * Resource identifier class.
 *
 * @param id the string representation of the identifier.
 * @tparam T the type of resource which is being identified
 */
case class Id[+T](id: String) {
    override def toString = id
}

object Id {
    def apply[T]: Id[T] =  new Id[T](UUID.randomUUID().toString)
    def apply[T](uuid: UUID): Id[T] = new Id[T](uuid.toString)
    def apply[T](prefix: String, id: Long) = new Id[T](s"$prefix-$id")
}

/**
 * Resource abstract class. This abstract class represents the state of any
 * infrastructure resource.
 *
 * @param id the identifier of the resource
 * @param name the (human) name of the resource
 */
sealed abstract class Resource(id: Id[_ <: Resource], name: String)

object MachineProfile extends Enumeration {
    type MachineProfile = Value

    val XS = Value(1, "XS machine")
    val S = Value(2, "S machine")
    val M = Value(3, "M machine")
    val X = Value(4, "X machine")
    val XL = Value(5, "XL machine")
}

object MachineStatus extends Enumeration {
    type MachineStatus = Value
    val Provisioning = Value(1, "provisioning")
    val Running = Value(2, "running")
}

/**
 * A class representing the state of a machine resource.
 *
 * @param id the identifier of the resource
 * @param name the (human) name of the resource
 * @param profile the profile of the machine
 * @param status the status of the machine
 */
case class MachineState(
                   id: Id[MachineState],
                   name: String,
                   profile: MachineProfile.Value,
                   status: MachineStatus.Value,
                   hostname: String,
                   ipAddress: String) extends Resource(id, name) {

    override def toString: String =
        s"{ id: '$id', name: '$name', profile: '$profile', status: " +
            s"'$status', hostname: '$hostname', ipAddress: '$ipAddress' }"
}
