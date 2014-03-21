/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.admin.command

import java.io.{StringWriter, PrintWriter}
import java.util.concurrent.TimeoutException
import scala.concurrent.{Future, Await}
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.duration.Duration
import scala.util.control.NonFatal
import scalaz.Validation

/** Represents the result of a command */
case class CommandResult(exitStatus: Int, message: String) {
  def isSuccess: Boolean = exitStatus == CommandResult.SuccessStatus
}

object CommandResult {
  val SuccessStatus = 0
  val InvalidArgsStatus = 1
  val ExecutionErrorStatus = -1

  /** Factory of successful command results */
  def success(message: String = "Command completed successfully"): CommandResult =
    CommandResult(CommandResult.SuccessStatus, message)

  /** Factory of failed command results */
  def error(
      message: String = "Command failed",
      exitStatus: Int = CommandResult.ExecutionErrorStatus): CommandResult =
    CommandResult(exitStatus, message)

  /** Translate an unexpected exception into a failed command result */
  def fromThrowable(throwable: Throwable): CommandResult = {
    val str = new StringWriter
    val writer = new PrintWriter(str)
    writer.println("Uncaught exception: %s", throwable.getMessage)
    throwable.printStackTrace(writer)
    CommandResult.error(str.toString)
  }

  def await(command: Future[CommandResult], timeout: Duration = Duration.Inf): CommandResult = {
    val commandWithErrorHandling = command.recover {
      case NonFatal(ex) => CommandResult.fromThrowable(ex)
    }
    try {
      Await.result(commandWithErrorHandling, timeout)
    } catch {
      case _: TimeoutException => CommandResult.error(s"Command timed out after $timeout")
    }
  }

  def fromValidation(validation: Validation[String, CommandResult]): CommandResult =
    validation.fold(
      fail = cause => CommandResult.error(cause),
      succ = identity
    )

  def fromBlock(block: => Boolean): CommandResult = try {
    if (block) CommandResult.success()
    else CommandResult.error()
  } catch {
    case NonFatal(ex) => CommandResult.fromThrowable(ex)
  }
}
