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

package es.tid.cosmos.api.report

import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.Future

import play.Logger

import es.tid.cosmos.api.email.{Email, Emailer}
import es.tid.cosmos.servicemanager.clusters.{ClusterId, Failed, ImmutableClusterDescription}

/** Class for reporting cluster failures.
  * Failures are logged and sent as an email to the provided address.
  *
  * @constructor           create a new cluster reporter
  * @param reportToAddress the email address to send the failure reports
  * @param emailer         the emailer to use for sending report emails
  */
class ClusterReporter(reportToAddress: String, emailer: Emailer) {

  /** Generate a report based on the cluster's future state.
    * This can be used to hook reporting to asynchronous cluster operations that return futures.
    * The report will be generated once the future description is available and only if the
    * cluster's state is Failed.
    *
    * @param id            the cluster ID
    * @param description_> the future description of the cluster.
    *                      This is meant to be the description after a certain operation has finished.
    * @param context       A textual description of the cluster operation that was performed whose
    *                      possible failure should be reported, e.g. provision a new cluster
    * @return              The future of the report's completion.
    */
  def reportOnFailure(
      id: ClusterId,
      description_> : Future[Option[ImmutableClusterDescription]],
      context: String): Future[Unit] = {

    def reportOnFailedState(description: Option[ImmutableClusterDescription]) {
      if (description.isEmpty) clusterNotFound(id)
      else description.get.state match {
        case Failed(error) => clusterFailed(description.get, error)
        case _ => () // all went well so do not report anything
      }
    }

    def clusterFailed(description: ImmutableClusterDescription, error: String) {
      val subject = s"[Cosmos] Cluster [${description.name}] failed"
      val message = s"""Cluster Details:
        |Name: ${description.name}
        |Id: ${description.id}
        |Size: ${description.size}
        |Master: ${description.master.map(h => s"${h.hostname} / ${h.ipAddress}").getOrElse("N/A")}
        |
        |Failure:
        |$error
        |
        |Context:
        |$context""".stripMargin

      logAndEmail(Email(reportToAddress, subject, message))
    }

    def unexpectedError(error: Throwable) = {
      val subject = "[Cosmos] Unexpected cluster operation failure"
      val message = s"""Cluster operation failed to provide a cluster description.
                        |Check if it uses withFailSafe() to wrap the errors.
                        |
                        |Cause:
                        |${error.getMessage}
                        |
                        |Context:
                        |$context""".stripMargin
      logAndEmail(Email(reportToAddress, subject, message))
      error
    }

    def clusterNotFound(id: ClusterId) {
      val subject = "[Cosmos] Cluster not found"
      val message = s"""Cluster with id [$id] was not found.
                        |
                        |Context:
                        |$context""".stripMargin
      logAndEmail(Email(reportToAddress, subject, message))
    }

    def logAndEmail(mail: Email) {
      emailer.email(mail)
      Logger.error(s"${mail.subject}: ${mail.message.replaceAll("\n", ", ")}")
    }

    description_>.transform(
      s = reportOnFailedState,
      f = unexpectedError
    )
  }
}
