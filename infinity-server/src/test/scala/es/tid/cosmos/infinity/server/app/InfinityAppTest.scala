package es.tid.cosmos.infinity.server.app

import java.net.InetAddress

import akka.actor._
import akka.testkit.{TestKit, TestProbe}
import org.scalatest.{BeforeAndAfterAll, FlatSpec}
import org.scalatest.matchers.MustMatchers
import spray.http._
import spray.http.HttpHeaders.`Remote-Address`

import es.tid.cosmos.infinity.server.authentication.AuthenticationComponent
import es.tid.cosmos.infinity.server.authorization.AuthorizationComponent
import es.tid.cosmos.infinity.server.processors.{Request, RequestProcessorComponent}
import es.tid.cosmos.infinity.test.MockActor

class InfinityAppTest extends TestKit(ActorSystem(classOf[InfinityAppTest].getSimpleName))
  with FlatSpec with MustMatchers with BeforeAndAfterAll {

  override def afterAll() = TestKit.shutdownActorSystem(system)

  val probe = TestProbe()
  val app = system.actorOf(TestInfinityAppComponent.infinityAppProps)
  var servicesCreated: Seq[MockActor.Created] = _
  val remoteAddress = InetAddress.getByName("10.0.0.1")
  val httpRequest = HttpRequest(
    method = HttpMethods.DELETE,
    uri = Uri("/webhdfs/v1/tmp/foo?op=DELETE&recursive=true")
  ).withHeaders(List(`Remote-Address`(RemoteAddress(remoteAddress))))


  object TestInfinityAppComponent extends InfinityAppComponent with AuthenticationComponent
    with AuthorizationComponent with RequestProcessorComponent{
    override val authenticationProps = MockActor.props("authentication", probe)
    override val authorizationProps = MockActor.props("authorization", probe)
    override def requestProcessorProps(authenticationRef: ActorRef, authorizationRef: ActorRef) = {
      servicesCreated.map(_.ref).toSet must be (Set(authenticationRef, authorizationRef))
      MockActor.props("request", probe)
    }
  }

  "An infinity app" must "spawn authentication and authorization components" in {
    servicesCreated =
      probe.expectMsgAllClassOf(classOf[MockActor.Created], classOf[MockActor.Created])
    servicesCreated.map(_.name).toSet must be (Set("authentication", "authorization"))
  }

  it must "spawn a request processor per request" in {
    probe.send(app, httpRequest)
    probe.expectMsgClass(classOf[MockActor.Created])
  }

  it must "delegate requests to it" in {
    val request = probe.expectMsgPF() {
      case MockActor.Received(r: Request, `app`) => r
    }
    request.remoteAddress must be (remoteAddress)
    request.httpRequest must be (httpRequest)
  }
}
