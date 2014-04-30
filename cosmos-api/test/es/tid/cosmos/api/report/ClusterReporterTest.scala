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

import java.net.URI
import scala.concurrent.Future

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.email.{Email, Emailer}
import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.servicemanager.ClusterName
import es.tid.cosmos.servicemanager.clusters._

class ClusterReporterTest extends FlatSpec with MustMatchers with FutureMatchers {

  "A cluster reporter" must "report cluster failures by email" in new WithReporter {
    val description_> = Future.successful(Some(failedClusterDescription))
    reporter.reportOnFailure(
      clusterId,
      description_>,
      "The context of the operation") must eventuallySucceed
    emailer.emailsSent must be (List(Email(
      to = "report@acme.com",
      subject = "[Cosmos][test environment] Cluster [failedCluster] failed",
      message =
        """Cosmos Master: master.acme.com
          |
          |Cluster Details:
          |Name: failedCluster
          |Id: anID
          |Size: 3
          |Master: host / ipAddress
          |
          |Failure:
          |This is an error
          |
          |Context:
          |The context of the operation""".stripMargin
    )))
  }

  it must "fail and report unexpected errors when future description fails" in new WithReporter {
    val description_> = Future.failed(new IllegalArgumentException("A cause"))
    reporter.reportOnFailure(
      clusterId, description_>, "a context") must eventuallyFailWith[IllegalArgumentException]
    emailer.emailsSent must have length 1
    val email = emailer.emailsSent.head
    email.to must be ("report@acme.com")
    email.subject must be ("[Cosmos][test environment] Unexpected cluster operation failure")
    email.message must (
      include ("Cosmos Master: master.acme.com") and
      include ("Cluster operation failed to provide a cluster description.") and
      include ("Check if it uses withFailSafe() to wrap the errors.") and
      include ("Cause:") and
      include ("A cause") and
      include ("Trace:") and
      include ("Context:") and
      include ("a context")
    )
  }

  it must "report an error when a cluster is not found" in new WithReporter {
    val description_> = Future.successful(None)
    reporter.reportOnFailure(clusterId, description_>, "a context") must eventuallySucceed
    emailer.emailsSent must be (List(Email(
      to = "report@acme.com",
      subject = "[Cosmos][test environment] Cluster not found",
      message =
        """Cosmos Master: master.acme.com
          |
          |Cluster with id [anID] was not found.
          |
          |Context:
          |a context""".stripMargin
    )))
  }

  it must "do nothing when cluster not in failed state" in new WithReporter {
    val description_> = Future.successful(Some(failedClusterDescription.copy(state = Running)))
    reporter.reportOnFailure(clusterId, description_>, "a context") must eventuallySucceed
    emailer.emailsSent must be ('empty)
  }



  trait WithReporter {
    val emailer = new MockEmailer
    val reporter = new ClusterReporter(
      cosmosEnvironment = "test environment",
      masterHost = "master.acme.com",
      reportToAddress = "report@acme.com",
      emailer
    )
    val clusterId = ClusterId("anID")
    val failedClusterDescription = ImmutableClusterDescription(
      id = clusterId,
      name = ClusterName("failedCluster"),
      state = Failed("This is an error"),
      size = 3,
      nameNode = Some(new URI("hdfs://host:1234")),
      master = Some(HostDetails("host", "ipAddress")),
      slaves  = Seq(HostDetails("host2", "ipAddress2"), HostDetails("host3", "ipAddress3")),
      users = None,
      services = Set("HDFS"),
      blockedPorts = Set(2, 4, 6)
    )
  }

  class MockEmailer extends Emailer {
    var emailsSent = List.empty[Email]
    override def email(mail: Email): Unit = {
      emailsSent = emailsSent :+ mail
    }
  }
}
