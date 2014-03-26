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

class InfinityAppTest extends TestKit(ActorSystem(classOf[InfinityAppTest].getSimpleName))
  with FlatSpec with MustMatchers with BeforeAndAfterAll {

  override def afterAll() = TestKit.shutdownActorSystem(system)

  val probe = TestProbe()

  case class ActorCreated(name: String, ref: ActorRef)
  case class Received(message: Any, sender: ActorRef)

  class MockActor(name: String) extends Actor {
    override def preStart(): Unit = probe.ref ! ActorCreated(name, self)
    override def receive: Receive = {
      case message => probe.ref ! Received(message, sender)
    }
  }

  object TestInfinityAppComponent extends InfinityAppComponent with AuthenticationComponent
    with AuthorizationComponent with RequestProcessorComponent{
    override val authenticationProps = Props(new MockActor("authentication"))
    override val authorizationProps = Props(new MockActor("authorization"))
    override def requestProcessorProps(authenticationRef: ActorRef, authorizationRef: ActorRef) = {
      servicesCreated.map(_.ref).toSet must be (Set(authenticationRef, authorizationRef))
      Props(new MockActor("request"))
    }
  }

  val app = system.actorOf(TestInfinityAppComponent.infinityAppProps)
  var servicesCreated: Seq[ActorCreated] = _
  val remoteAddress = InetAddress.getByName("10.0.0.1")
  val httpRequest = HttpRequest(
    method = HttpMethods.DELETE,
    uri = Uri("/webhdfs/v1/tmp/foo?op=DELETE&recursive=true")
  ).withHeaders(List(`Remote-Address`(RemoteAddress(remoteAddress))))

  "An infinity app" must "spawn authentication and authorization components" in {
    servicesCreated = probe.expectMsgAllClassOf(classOf[ActorCreated], classOf[ActorCreated])
    servicesCreated.map(_.name).toSet must be (Set("authentication", "authorization"))
  }

  it must "spawn a request processor per request" in {
    probe.send(app, httpRequest)
    probe.expectMsgClass(classOf[ActorCreated])
  }

  it must "delegate requests to it" in {
    val request = probe.expectMsgPF() {
      case Received(r: Request, `app`) => r
    }
    request.remoteAddress must be (remoteAddress)
    request.httpRequest must be (httpRequest)
  }
}
