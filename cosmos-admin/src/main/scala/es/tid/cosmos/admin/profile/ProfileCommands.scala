package es.tid.cosmos.admin.profile

import es.tid.cosmos.admin.command.CommandResult

trait ProfileCommands {
  def list(): CommandResult
  def setMachineQuota(handle: String, limit: Int): CommandResult
  def removeMachineQuota(handle: String): CommandResult
  def enableCapability(handle: String, capability: String): CommandResult
  def disableCapability(handle: String, capability: String): CommandResult
  def setGroup(handle: String, groupName: String): CommandResult
  def removeGroup(handle: String): CommandResult
}
