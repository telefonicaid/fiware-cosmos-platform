package org.apache.hadoop.hdfs.web

import org.apache.hadoop.hdfs.web.resources.{Param, HttpOpParam}
import org.apache.hadoop.fs.Path
import java.net.{URI, URL}
import org.apache.hadoop.conf.Configuration

class SecureWebHdfsFS extends WebHdfsFileSystem {
  import SecureWebHdfsFS._

  var secret: String = _

  override def initialize(uri: URI, conf: Configuration) = synchronized {
    super.initialize(uri, conf)
    secret = conf.get(ClusterSecretKey)
    require(secret != null, "Hadoop configuration does not contain a cluster secret")
  }

  override def toUrl(op: HttpOpParam.Op, fspath: Path, parameters: Param[_, _]*) = {
    val url = super.toUrl(op, fspath, parameters: _*)
    val authority = url.getAuthority
    assert(authority != null)
    val path = url.getPath
    assert(path != null)
    val query = "?" + Option(url.getQuery).map(_ + "&").getOrElse("") + "secret=" + secret
    val ref = Option(url.getRef).map("#" + _).getOrElse("")
    new URL(s"http://$authority$path$query$ref")
  }
}

object SecureWebHdfsFS {
  val ClusterSecretKey = "es.tid.cosmos.clustersecret"
}
