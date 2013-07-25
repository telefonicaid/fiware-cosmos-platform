package es.tid.cosmos.servicemanager

import java.net.URI
import scala.concurrent.{Future, promise}
import scala.concurrent.duration._

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.platform.common.scalatest.matchers.FutureMatchers

class MutableClusterDescriptionTest extends FlatSpec with MustMatchers with FutureMatchers {

  val TestTimeout = 1 seconds

  trait SampleMutableClusterDescription {
    private val deploymentPromise = promise[Unit]

    val description = new MutableClusterDescription(
      id = ClusterId(),
      name = "Sample cluster",
      size = 100,
      deployment_> = deploymentPromise.future,
      machines_> = Future.successful(Seq()),
      nameNode_> = Future.successful(new URI("http://host:8000"))
    )

    def finishDeployment() = deploymentPromise.success(())
  }

  "A mutable cluster description" must "provide a future of its next state" in
    new SampleMutableClusterDescription {
      description.state must equal (Provisioning)
      val state_> = description.nextState
      finishDeployment()
      state_>  must runUnder(TestTimeout)
      state_>  must eventually(equal(Running))
    }

  "Its future for a target state" must "succeed when in that state" in
    new SampleMutableClusterDescription {
      description.state must equal (Provisioning)
      description.whenInState(Provisioning) must runUnder(TestTimeout)
    }

  it must "succeed when target state is reached" in
    new SampleMutableClusterDescription {
      description.state must equal (Provisioning)
      val whenTerminated_> = description.whenInState(Running)
      finishDeployment()
      whenTerminated_> must runUnder(TestTimeout)
    }

  it must "fail when state transitions exceed a maximum before reaching target state" in
    new SampleMutableClusterDescription {
      description.state = Running
      val whenTerminated_> = description.whenInState(Terminated, 1)
      description.state = Terminating
      description.state = Terminated
      whenTerminated_> must eventuallyFailWith [IllegalStateException]
    }
}
