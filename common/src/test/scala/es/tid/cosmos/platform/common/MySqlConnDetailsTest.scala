package es.tid.cosmos.platform.common

import com.typesafe.config.ConfigFactory
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class MySqlConnDetailsTest extends FlatSpec with MustMatchers {
  "MySQL connection details" must "be loadable from a complete configuration" in {
    val config = ConfigFactory.parseString(
      """ial.db.host=host1
        |ial.db.port=1004
        |ial.db.username=user
        |ial.db.password=pwd
        |ial.db.name=database
      """.stripMargin)
    MySqlConnDetails.fromConfig(config) must be (MySqlConnDetails(
      host="host1", port=1004, username= "user", password= "pwd", dbName="database"))
  }
}
