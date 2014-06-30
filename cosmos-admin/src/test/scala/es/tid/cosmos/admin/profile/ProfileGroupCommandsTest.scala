package es.tid.cosmos.admin.profile

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManagerComponent
import es.tid.cosmos.api.profile.{ClusterSecret, ProfileId}
import es.tid.cosmos.api.profile.CosmosProfileTestHelpers._
import es.tid.cosmos.api.profile.dao.mock.MockCosmosDataStoreComponent
import es.tid.cosmos.api.quota.{NoGroup, Quota, GuaranteedGroup}
import es.tid.cosmos.servicemanager.ClusterUser
import es.tid.cosmos.servicemanager.clusters.Running

class ProfileGroupCommandsTest extends FlatSpec with MustMatchers {

  trait WithGroupCommands extends MockCosmosDataStoreComponent
      with MockedServiceManagerComponent {
    mockedServiceManager.deployPersistentHdfsCluster(null)
    val group = GuaranteedGroup("mygroup", Quota(3))
    val ownerHandle = "owner"
    val handle = "jsmith"
    val (ownerId, cosmosId) = store.withTransaction { implicit c =>
      store.group.register(group)
      (registerUser(store, ownerHandle).id, registerUser(store, handle).id)
    }
    private val initialUsers = Seq(ClusterUser.enabled(handle, None, "somepublickey"),
      ClusterUser.enabled(ownerHandle, None, "somepublickey"))
    mockedServiceManager.setUsers(serviceManager.persistentHdfsId, initialUsers)
    val groupCommands = new ProfileGroupCommands(store, serviceManager)


    def addToGroup(profileIds: ProfileId*): Unit = store.withTransaction { implicit c =>
      profileIds.foreach(id => store.profile.setGroup(id, group))
      val usernames = profileIds.map(store.profile.lookupByProfileId).flatten.map(_.handle)
      val oldUsers = mockedServiceManager.describePersistentHdfsCluster().get.users.get
      val usersToChange = oldUsers.filter(usernames.contains)
      val newUsers = (oldUsers -- usersToChange) ++ usersToChange.map(_.copy(group = group.hdfsGroupName))
      mockedServiceManager.setUsers(mockedServiceManager.persistentHdfsId, newUsers.toSeq)
    }

    def createSharedCluster(owner: String, withAccess: Set[String] = Set.empty) =
      store.withTransaction { implicit c =>
        val clusterUsers = (withAccess + owner).map(ClusterUser(_, Some("group"), "ssh-rsa XXXX"))
        val fakeCluster = mockedServiceManager.defineCluster(
          users = clusterUsers,
          initialState = Some(Running)
        )
        val ownerId = store.profile.lookupByHandle(owner).get.id
        store.cluster.register(fakeCluster.view.id, ownerId, ClusterSecret.random(), shared = true)
        fakeCluster
      }

    def currentGroup() = store.withTransaction { implicit c =>
      store.profile.lookupByHandle(handle)
    }.get.group

    def currentInfinityGroup() = mockedServiceManager.describePersistentHdfsCluster()
      .get.users.get.find(_.username == handle).get.group
  }

  "A profile group command" must "set to an existing group" in new WithGroupCommands {
    store.withTransaction { implicit c =>
      store.group.register(group)
      groupCommands.set(handle, group.name) must be ('success)
      currentGroup() must be (group)
      currentInfinityGroup() must be (group.hdfsGroupName)
    }
  }

  it must "remove user from its assigned group" in new WithGroupCommands {
    addToGroup(cosmosId)
    groupCommands.remove(handle) must be ('success)
    store.withTransaction { implicit c =>
      currentGroup() must be (NoGroup)
      currentInfinityGroup() must be (NoGroup.hdfsGroupName)
    }
  }

  it must "reject removing user from group if he owns a shared cluster" in
    new WithGroupCommands {
      val clusterId = mockedServiceManager.defineCluster().view.id
      addToGroup(cosmosId)
      store.withTransaction { implicit c =>
        store.cluster.register(clusterId, cosmosId, ClusterSecret.random(), shared = true)
      }
      val result = groupCommands.remove(handle)
      result must not be 'success
      result.message must include (clusterId.toString)
    }

  it must "disable user from shared clusters when leaving a group" in
    new WithGroupCommands {
      addToGroup(ownerId, cosmosId)
      val cluster = createSharedCluster(owner = ownerHandle, withAccess = Set(handle))
      groupCommands.remove(handle) must be ('success)
      val disabledUser = cluster.view.users.get.find(_.username == handle)
      disabledUser.map(_.isEnabled) must be (Some(false))
    }

  it must "add user to shared clusters when entering a group" in
    new WithGroupCommands {
      addToGroup(ownerId)
      val cluster = createSharedCluster(owner = ownerHandle)
      groupCommands.set(handle, group.name) must be ('success)
      cluster.view.users.get.collectFirst {
        case user if user.username == handle && user.isEnabled => user
      } must be ('defined)
    }
}
