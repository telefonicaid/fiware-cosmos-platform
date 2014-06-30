package es.tid.cosmos.admin.profile

import es.tid.cosmos.admin.command.CommandResult
import es.tid.cosmos.api.profile.UserState
import es.tid.cosmos.api.profile.dao.ProfileDataStore

private[profile] class ListProfilesCommand(store: ProfileDataStore) {

  def apply(): CommandResult = CommandResult.success {
    val handles = currentHandles()
    if (handles.isEmpty) "No users found"
    else s"Users found (handles):\n${handles.sorted.mkString("\n")}"
  }

  private def currentHandles(): Seq[String] = store.withTransaction { implicit c =>
    for (profile <- store.profile.list() if profile.state != UserState.Deleted) yield profile.handle
  }
}
