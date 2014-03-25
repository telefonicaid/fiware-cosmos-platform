package org.apache.hadoop.hdfs.web

import java.net.{URL, URI}

import org.apache.hadoop.conf.Configuration
import org.apache.hadoop.fs.Path
import org.apache.hadoop.hdfs.web.resources.GetOpParam
import org.apache.hadoop.security.UserGroupInformation
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

class SecureWebHdfsFSTest extends FlatSpec with MustMatchers with MockitoSugar {

  "A secure WebHDFS" must "correctly add the secret query parameter" in {
    val instance = new SecureWebHdfsFS
    val config = new Configuration
    config.set(SecureWebHdfsFS.ClusterSecretKey, "secret")
    instance.initialize(new URI("webhdfs://some-host"), config)
    val result = instance.toUrl(GetOpParam.Op.LISTSTATUS, new Path("webhdfs://some-host:50070/user/turing"))
    val user = UserGroupInformation.getCurrentUser.getShortUserName
    result must be (new URL(s"http://some-host:50070/webhdfs/v1/user/turing?op=LISTSTATUS&user.name=$user&secret=secret"))
  }

  it must "fail to initialize if the secret is not present" in {
    val instance = new SecureWebHdfsFS
    evaluating {
      instance.initialize(new URI("webhdfs://some-host"), new Configuration)
    } must produce[IllegalArgumentException]
  }
}
