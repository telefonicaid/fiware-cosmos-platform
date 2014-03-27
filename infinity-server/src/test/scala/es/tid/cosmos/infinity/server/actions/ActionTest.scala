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

package es.tid.cosmos.infinity.server.actions

import java.net.InetAddress

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import spray.httpx.RequestBuilding
import spray.http.{HttpMethods, HttpMethod}

import es.tid.cosmos.infinity.server.authentication.{ClusterCredentials, UserCredentials}
import es.tid.cosmos.infinity.server.util.Path

class ActionTest extends FlatSpec with MustMatchers with RequestBuilding {

  val remoteAddress = InetAddress.getByName("10.0.0.1")

  "Action" must "be instantiated from user authentication request" in {
    val action = Action(
      remoteAddress,
      request = uriWithParams(
        HttpMethods.GET,
        "op" -> "GETFILESTATUS",
        "user.name" -> "apv",
        "api.key" -> "key",
        "api.secret" -> "secret"
      )
    )
    action.credentials must be (UserCredentials("key", "secret"))
  }

  it must "be instantiated from cluster authentication request" in {
    val action = Action(
      remoteAddress,
      request = uriWithParams(
        HttpMethods.GET,
        "op" -> "GETFILESTATUS",
        "user.name" -> "apv",
        "cluster.secret" -> "secret"
      )
    )
    action.credentials must be (ClusterCredentials("10.0.0.1", "secret"))
  }

  it must "fail to be instantiated from request with missing auth params" in {
    evaluating { Action(
      remoteAddress,
      request = uriWithParams(
        HttpMethods.GET,
        "op" -> "GETFILESTATUS",
        "user.name" -> "apv"
      )
    )} must produce [IllegalArgumentException]
  }

  it must "fail to be instantiated from user authentication request with missing API key param" in {
    evaluating { Action(
      remoteAddress,
      request = uriWithParams(
        HttpMethods.GET,
        "op" -> "GETFILESTATUS",
        "user.name" -> "apv",
        "api.secret" -> "secret"
      )
    )} must produce [IllegalArgumentException]
  }

  it must "fail to be instantiated from user authentication request with missing API secret param" in {
    evaluating { Action(
      remoteAddress,
      request = uriWithParams(
        HttpMethods.GET,
        "op" -> "GETFILESTATUS",
        "user.name" -> "apv",
        "api.key" -> "key"
      )
    )} must produce [IllegalArgumentException]
  }

  it must "fail to be instantiated from request with mixing user & authentication params" in {
    evaluating { Action(
      remoteAddress,
      request = uriWithParams(
        HttpMethods.GET,
        "op" -> "GETFILESTATUS",
        "user.name" -> "apv",
        "api.key" -> "key",
        "api.secret" -> "secret",
        "cluster.secret" -> "secret"
      )
    )} must produce [IllegalArgumentException]
  }

  it must "fail to be instantiated from unknown 'op' param" in {
    evaluating { Action(
      remoteAddress,
      request = uriWithParams(
        HttpMethods.GET,
        "op" -> "UNKNOWN"
      )
    )} must produce [IllegalArgumentException]
  }

  it must "fail to be instantiated from missing 'op' param" in {
    evaluating { Action(
      remoteAddress,
      request = uriWithParams(HttpMethods.GET)
    )} must produce [IllegalArgumentException]
  }

  it must "be instantiated from OPEN request" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpGetWithParams("OPEN",
        "offset" -> "1024",
        "length" -> "500",
        "buffersize" -> "4096"
      )
    )
    action must be (Action.Open(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials,
      offset = Some(1024),
      length = Some(500),
      buffersize = Some(4096)
    ))
  }

  it must "be instantiated from OPEN request with missing optional params" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpGetWithParams("OPEN")
    )
    action must be (Action.Open(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials))
  }

  it must behave like failingOnMethodsOtherThan(HttpMethods.GET, "OPEN",
    "offset" -> "1024",
    "length" -> "500",
    "buffersize" -> "4096"
  )

  it must "be instantiated from GETFILESTATUS request" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpGetWithParams("GETFILESTATUS")
    )
    action must be (Action.GetFileStatus(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials
    ))
  }

  it must behave like failingOnMethodsOtherThan(HttpMethods.GET, "GETFILESTATUS")

  it must "be instantiated from LISTSTATUS request" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpGetWithParams("LISTSTATUS")
    )
    action must be (Action.ListStatus(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials
    ))
  }

  it must behave like failingOnMethodsOtherThan(HttpMethods.GET, "LISTSTATUS")

  it must "be instantiated from GETCONTENTSUMMARY request" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpGetWithParams("GETCONTENTSUMMARY")
    )
    action must be (Action.GetContentSummary(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials
    ))
  }

  it must behave like failingOnMethodsOtherThan(HttpMethods.GET, "GETCONTENTSUMMARY")

  it must "be instantiated from GETFILECHECKSUM request" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpGetWithParams("GETFILECHECKSUM")
    )
    action must be (Action.GetFileChecksum(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials
    ))
  }

  it must behave like failingOnMethodsOtherThan(HttpMethods.GET, "GETFILECHECKSUM")

  it must "be instantiated from GETHOMEDIRECTORY request" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpGetWithParams("GETHOMEDIRECTORY")
    )
    action must be (Action.GetHomeDirectory(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials
    ))
  }

  it must behave like failingOnMethodsOtherThan(HttpMethods.GET, "GETHOMEDIRECTORY")

  it must "be instantiated from GETDELEGATIONTOKEN request" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpGetWithParams("GETDELEGATIONTOKEN",
        "renewer" -> "pepito"
      )
    )
    action must be (Action.GetDelegationToken(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials,
      renewer = "pepito"
    ))
  }

  it must "fail to be instantiated from GETDELEGATIONTOKEN request with missing required params" in {
    evaluating { Action(
      remoteAddress,
      request = authenticatedHttpGetWithParams("GETDELEGATIONTOKEN")
    )} must produce [IllegalArgumentException]
  }

  it must behave like failingOnMethodsOtherThan(
    HttpMethods.GET, "GETDELEGATIONTOKEN", "renewer" -> "pepito")

  it must "be instantiated from CREATE request" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpPutWithParams("CREATE",
        "overwrite" -> "true",
        "blocksize" -> "4096",
        "replication" -> "3",
        "permission" -> "755",
        "buffersize" -> "1024"
      )
    )
    action must be (Action.Create(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials,
      overwrite = Some(true),
      blocksize = Some(4096),
      replication = Some(3),
      permission = Some(Integer.parseInt("755", 8)),
      buffersize = Some(1024)
    ))
  }

  it must "be instantiated from CREATE request with omitted optional params" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpPutWithParams("CREATE")
    )
    action must be (Action.Create(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials
    ))
  }

  it must behave like failingOnMethodsOtherThan(HttpMethods.PUT, "CREATE")

  it must "be instantiated from MKDIRS request" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpPutWithParams("MKDIRS",
        "permission" -> "755"
      )
    )
    action must be (Action.Mkdirs(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials,
      permission = Some(Integer.parseInt("755", 8))
    ))
  }

  it must "be instantiated from MKDIRS request with omitted optional params" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpPutWithParams("MKDIRS")
    )
    action must be (Action.Mkdirs(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials
    ))
  }

  it must behave like failingOnMethodsOtherThan(HttpMethods.PUT, "MKDIRS")

  it must "be instantiated from RENAME request" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpPutWithParams("RENAME",
        "destination" -> "/bar/foo"
      )
    )
    action must be (Action.Rename(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials,
      destination = "/bar/foo"
    ))
  }

  it must "fail to be instantiated from RENAME request with missing required params" in {
    evaluating { Action(
      remoteAddress,
      request = authenticatedHttpPutWithParams("RENAME")
    )} must produce [IllegalArgumentException]
  }

  it must behave like failingOnMethodsOtherThan(
    HttpMethods.PUT, "RENAME", "destination" -> "/bar/foo")

  it must "be instantiated from SETREPLICATION request" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpPutWithParams("SETREPLICATION",
        "replication" -> "2"
      )
    )
    action must be (Action.SetReplication(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials,
      replication = Some(2)
    ))
  }

  it must "be instantiated from SETREPLICATION request with omitted optional params" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpPutWithParams("SETREPLICATION")
    )
    action must be (Action.SetReplication(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials
    ))
  }

  it must behave like failingOnMethodsOtherThan(HttpMethods.PUT, "SETREPLICATION")

  it must "be instantiated from SETOWNER request" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpPutWithParams("SETOWNER",
        "owner" -> "gandalf",
        "group" -> "istari"
      )
    )
    action must be (Action.SetOwner(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials,
      owner = Some("gandalf"),
      group = Some("istari")
    ))
  }

  it must "be instantiated from SETOWNER request with omitted optional params" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpPutWithParams("SETOWNER")
    )
    action must be (Action.SetOwner(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials
    ))
  }

  it must behave like failingOnMethodsOtherThan(HttpMethods.PUT, "SETOWNER")

  it must "be instantiated from SETPERMISSION request" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpPutWithParams("SETPERMISSION",
        "permission" -> "755"
      )
    )
    action must be (Action.SetPermission(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials,
      permission = Some(Integer.parseInt("755", 8))
    ))
  }

  it must "be instantiated from SETPERMISSION request with omitted optional params" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpPutWithParams("SETPERMISSION")
    )
    action must be (Action.SetPermission(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials
    ))
  }

  it must behave like failingOnMethodsOtherThan(HttpMethods.PUT, "SETPERMISSION")

  it must "be instantiated from SETTIMES request" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpPutWithParams("SETTIMES",
        "modificationtime" -> "1395656039",
        "accesstime" -> "1395656074"
      )
    )
    action must be (Action.SetTimes(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials,
      modificationTime = Some(1395656039),
      accessTime = Some(1395656074)
    ))
  }

  it must "be instantiated from SETTIMES request with omitted optional params" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpPutWithParams("SETTIMES")
    )
    action must be (Action.SetTimes(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials
    ))
  }

  it must behave like failingOnMethodsOtherThan(HttpMethods.PUT, "SETTIMES")

  it must "be instantiated from RENEWDELEGATIONTOKEN request" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpPutWithParams("RENEWDELEGATIONTOKEN",
        "token" -> "GbsDtWmD9XlnUUWbY/nhBveW8I"
      )
    )
    action must be (Action.RenewDelegationToken(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials,
      token = "GbsDtWmD9XlnUUWbY/nhBveW8I"
    ))
  }

  it must "fail to be instantiated from RENEWDELEGATIONTOKEN request with missing required params" in {
    evaluating { Action(
      remoteAddress,
      request = authenticatedHttpPutWithParams("RENEWDELEGATIONTOKEN")
    )} must produce [IllegalArgumentException]
  }

  it must behave like failingOnMethodsOtherThan(
    HttpMethods.PUT, "RENEWDELEGATIONTOKEN", "token" -> "GbsDtWmD9XlnUUWbY/nhBveW8I")

  it must "be instantiated from CANCELDELEGATIONTOKEN request" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpPutWithParams("CANCELDELEGATIONTOKEN",
        "token" -> "GbsDtWmD9XlnUUWbY/nhBveW8I"
      )
    )
    action must be (Action.CancelDelegationToken(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials,
      token = "GbsDtWmD9XlnUUWbY/nhBveW8I"
    ))
  }

  it must "fail to be instantiated from CANCELDELEGATIONTOKEN request with missing required params" in {
    evaluating { Action(
      remoteAddress,
      request = authenticatedHttpPutWithParams("CANCELDELEGATIONTOKEN")
    )} must produce [IllegalArgumentException]
  }

  it must behave like failingOnMethodsOtherThan(
    HttpMethods.PUT, "CANCELDELEGATIONTOKEN", "token" -> "GbsDtWmD9XlnUUWbY/nhBveW8I")

  it must "be instantiated from APPEND request" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpPostWithParams("APPEND",
        "buffersize" -> "65536"
      )
    )
    action must be (Action.Append(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials,
      buffersize = Some(65536)
    ))
  }

  it must "be instantiated from APPEND request with omitted optional params" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpPostWithParams("APPEND")
    )
    action must be (Action.Append(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials
    ))
  }

  it must behave like failingOnMethodsOtherThan(HttpMethods.POST, "APPEND")

  it must "be instantiated from DELETE request" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpDeleteWithParams("DELETE",
        "recursive" -> "true"
      )
    )
    action must be (Action.Delete(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials,
      recursive = Some(true)
    ))
  }

  it must "be instantiated from DELETE request with omitted optional params" in {
    val action = Action(
      remoteAddress,
      request = authenticatedHttpDeleteWithParams("DELETE")
    )
    action must be (Action.Delete(
      on = Path.absolute("/webhdfs/v1/foo/bar"),
      credentials = sampleCredentials
    ))
  }

  it must behave like failingOnMethodsOtherThan(HttpMethods.DELETE, "DELETE")

  private lazy val sampleCredentials = UserCredentials("key", "secret")

  private def uriWithParams(method: HttpMethod, params: (String, String)*) = {
    val query = params.map { case (k, v) => s"$k=$v" }.mkString("&")
    new RequestBuilder(method).apply(s"/webhdfs/v1/foo/bar?$query")
  }

  private def authenticatedUriWithParams(
      method: HttpMethod, op: String, params: (String, String)*) = uriWithParams(method, Seq(
    "op" -> s"$op",
    "user.name" -> "apv",
    "api.key" -> sampleCredentials.apiKey,
    "api.secret" -> sampleCredentials.apiSecret
  ) ++ params: _*)

  private def authenticatedHttpGetWithParams(op: String, params: (String, String)*) =
    authenticatedUriWithParams(HttpMethods.GET, op, params: _*)

  private def authenticatedHttpPutWithParams(op: String, params: (String, String)*) =
    authenticatedUriWithParams(HttpMethods.PUT, op, params: _*)

  private def authenticatedHttpPostWithParams(op: String, params: (String, String)*) =
    authenticatedUriWithParams(HttpMethods.POST, op, params: _*)

  private def authenticatedHttpDeleteWithParams(op: String, params: (String, String)*) =
    authenticatedUriWithParams(HttpMethods.DELETE, op, params: _*)

  private def failingOnMethodsOtherThan(
      validMethod: HttpMethod, op: String, params: (String, String)*) = {
    val invalidMethods = Set(
      HttpMethods.GET, HttpMethods.PUT, HttpMethods.POST, HttpMethods.DELETE) - validMethod
    invalidMethods.foreach { method =>
      it must s"fail to be instantiated on $op request with HTTP $method" in {
        evaluating {
          Action(
          remoteAddress,
          request = authenticatedUriWithParams(method, op, params: _*)
        )} must produce [IllegalArgumentException]
      }
    }
  }
}
