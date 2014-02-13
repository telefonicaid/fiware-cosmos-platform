import com.typesafe.sbt.packager.linux.Keys.linuxPackageMappings
import com.typesafe.sbt.packager.linux.Keys.maintainer
import com.typesafe.sbt.packager.linux.Keys.packageArchitecture
import com.typesafe.sbt.packager.linux.Keys.packageSummary
import com.typesafe.sbt.packager.Keys._
import com.typesafe.sbt.SbtNativePackager._
import java.text.SimpleDateFormat
import java.util.Date
import sbt._
import sbt.Keys._
import sbt.Keys.name
import sbt.Keys.packageBin
import sbt.Keys.streams
import sbt.Keys.target
import sbt.Keys.version
import scala.Some

object CosmosAPIBuild extends RpmHelper {

  val settings: Seq[Setting[_]] = rpmSettings

  private def rpmSettings: Seq[Setting[_]] = {
    Seq(
      dist <<= (packageBin in Rpm) dependsOn dist,
      name in Rpm := "cosmos-api",
      maintainer := "Cosmos Team <cosmos@tid.es>",
      packageSummary := "Cosmos API",
      description in Rpm := "Cosmos API server",
      rpmVendor := "Telefonica Digital",
      rpmGroup := Some("System Environment/Libraries"),
      rpmLicense := Some("All rights reserved"),
      version in Rpm <<= version apply { v =>
        v.replace("-SNAPSHOT", currentTimestamp)
      },
      packageArchitecture in Rpm := "noarch",
      rpmOs in Rpm:= "linux",
      rpmRequirements := Seq("java"),
      rpmAutoreq := "no",
      rpmAutoprov := "no",
      linuxPackageMappings in Rpm <<= (mappings in Universal) map { (f: Seq[(File,String)]) =>
        f map { case (file: File, name: String) =>
          packageMapping(file -> s"/opt/pdi-cosmos/cosmos-api/$name")
          //withUser "root" withGroup "root" withPerms "0755"
        }
      },
      linuxPackageMappings in Rpm <++= (baseDirectory) map { base =>
        Seq(
          packageMapping ( base / "scripts/cosmos-api" -> "/etc/init.d/cosmos-api"),
          packageMapping ( IO.temporaryDirectory / "." -> "/opt/pdi-cosmos/var/run/")
            withUser "root" withGroup "root" withPerms "0440"
        )
      },
      linuxPackageMappings in Rpm <++= (baseDirectory) map { base =>
        val egg = base.getParentFile / "cosmos-cli/dist" * "*.egg"
        egg.get map { f =>
          packageMapping ( f -> s"/opt/repos/eggs/${f.name}" )
        }
      },
      rpmPost := Some("""chmod +x /etc/init.d/cosmos-api
                        |chkconfig --add /etc/init.d/cosmos-api
                        |chkconfig cosmos-api on
                      """.stripMargin),
      packageBin in Rpm <<= (rpmSpecConfig in Rpm, target in Rpm, streams) map { (spec, dir, s) =>
        buildRpm(spec, dir, s.log)
      }
    )
  }

  private val currentTimestamp = new SimpleDateFormat(".yyyyMMdd.HHmmss").format(new Date)

}