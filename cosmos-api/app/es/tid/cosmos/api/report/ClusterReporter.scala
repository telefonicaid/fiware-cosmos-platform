/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
class ClusterReporter(
    cosmosEnvironment: String, masterHost: String, reportToAddress: String, emailer: Emailer) {

  private val titlePrefix: String = s"[Cosmos][$cosmosEnvironment]"
  private val masterHostPrefix: String = s"Cosmos Master: $masterHost"

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
      val subject = s"$titlePrefix Cluster [${description.name}] failed"
      val message = s"""$masterHostPrefix
        |
        |Cluster Details:
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
      val subject = s"$titlePrefix Unexpected cluster operation failure"
      val message = s"""$masterHostPrefix
                        |
                        |Cluster operation failed to provide a cluster description.
                        |Check if it uses withFailSafe() to wrap the errors.
                        |
                        |Cause:
                        |${error.getMessage}
                        |
                        |Trace:
                        |${error.getStackTrace.toList.mkString("\n")}
                        |
                        |Context:
                        |$context""".stripMargin
      logAndEmail(Email(reportToAddress, subject, message))
      error
    }

    def clusterNotFound(id: ClusterId) {
      val subject = s"$titlePrefix Cluster not found"
      val message = s"""$masterHostPrefix
                        |
                        |Cluster with id [$id] was not found.
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
