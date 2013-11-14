package es.tid.cosmos.api.profile

import scalaz._

import es.tid.cosmos.api.controllers.cluster.ClusterReference

/**
 * Class responsible for handling user quotas.
 *
 * @constructor           create a new profile-quotas handler.
 *                        ''Note:'' The constructor uses function and pass-by-name parameters to
 *                        allow loose-coupling while deferring their evaluation at the latest
 *                        possible moment.
 * @param machinePoolSize the total number of machines regardless of their usage
 * @param groups          the user groups
 * @param lookupByGroup   a function to lookup the users of a given group
 * @param listClusters    a function to get the clusters of a given user
 */
class ProfileQuotas(
  machinePoolSize: => Int,
  groups: => Set[Group],
  lookupByGroup: Group => Set[CosmosProfile],
  listClusters: CosmosProfile => List[ClusterReference]) {

  //TODO lookupByGroup -> listClusters could be simplified with a function
  // Group => List[Int] where Int represents the number of machines used by a group

  /**
   * Check if the given user can request the given number of machines based on their profile and
   * group quotas.
   *
   *
   * @param profile the user profile
   * @param size    the cluster size as a number of machines being requested for the user
   *                (apart from the ones they already might have)
   * @return        the validation result containing the requested cluster size if validation was
   *                 successful or the error messages in case of validation failures
   */
  def withinQuota(profile: CosmosProfile, size: Int): ValidationNel[String, Int] = {
    import scalaz.Scalaz._

    val availableFromProfile = profile.quota - Quota(usedMachines(profile))
    val availableFromGroup = maxAvailableFromGroup(profile.group)
    val overallAvailable = Quota.min(availableFromProfile, availableFromGroup)

    val profileValidation = validate(availableFromProfile, size, "Profile quota exceeded")
    val groupValidation = validate(
      availableFromGroup, size, s"Quota exceeded for group [${profile.group.name}].")
    val overallValidation = validate(
      overallAvailable, size,
      s"You can request up to ${overallAvailable.toInt.getOrElse(0)} machine(s) at this point.")

    (profileValidation |@| groupValidation |@| overallValidation){_ + _ + _}
  }

  private def maxAvailableFromGroup(group: Group): Quota = {
    val usedByGroups = usedMachinesByGroups(groups)
    val maxQuota = GroupQuotas.maximumQuota(group, usedByGroups, machinePoolSize)
    maxQuota - Quota(usedByGroups(group))
  }

  private def validate(quota: Quota, size: Int, errorMessage: String): ValidationNel[String, Int] = {
    import scalaz.Scalaz._
    if (quota.withinQuota(size)) size.successNel[String] else errorMessage.failureNel[Int]
  }

  private def usedMachinesByGroups(groups: Set[Group]): Map[Group, Int] =
    (for {
        group <- groups.toSeq
        profile <- lookupByGroup(group)
     } yield (group, usedMachines(profile))
    ).groupBy(_._1).mapValues(_.map(_._2)).mapValues(_.sum)

  private def usedMachines(profile: CosmosProfile): Int =
    listClusters(profile).map(_.description.size).sum
}
