package es.tid.cosmos.infinity.test

import akka.actor._
import akka.testkit.TestProbe

/** Mocked actor useful for injecting props to an actor under test. */
class MockActor(name: String, listener: ActorRef) extends Actor {
  import MockActor._

  override def preStart(): Unit = listener ! Created(name, self)

  override def receive: Receive = {
    case message if sender != listener => listener ! Received(message, sender)
  }
}

object MockActor {

  /** Mock actor will send this message to the listener when starting. */
  case class Created(name: String, ref: ActorRef)

  /** Every received message but the ones sent from the listener will be reported
    * to the listener wrapped in this class.
    */
  case class Received(message: Any, sender: ActorRef)

  def props(name: String, listener: ActorRef): Props = Props(new MockActor(name, listener))
  def props(name: String, listener: TestProbe): Props = props(name, listener.ref)
}
