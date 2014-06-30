package es.tid.cosmos.admin.profile

import es.tid.cosmos.admin.command.CommandResult

trait ProfileCommands {
  def list(): CommandResult
  def quota: ProfileCommands.QuotaCommands
  def capability: ProfileCommands.CapabilityCommands
  def group: ProfileCommands.GroupCommands
}

object ProfileCommands {

  trait QuotaCommands {
    def set(handle: String, limit: Int): CommandResult
    def remove(handle: String): CommandResult
  }

  trait CapabilityCommands {
    def enable(handle: String, capability: String): CommandResult
    def disable(handle: String, capability: String): CommandResult
  }

  trait GroupCommands {
    def set(handle: String, groupName: String): CommandResult
    def remove(handle: String): CommandResult
  }
}
