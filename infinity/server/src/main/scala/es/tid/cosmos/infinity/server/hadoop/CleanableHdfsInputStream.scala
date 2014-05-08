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

package es.tid.cosmos.infinity.server.hadoop

import org.apache.hadoop.fs.FSDataInputStream
import org.apache.hadoop.hdfs.client.HdfsDataInputStream
import java.io.IOException
import org.apache.hadoop.io.IOUtils
import org.apache.commons.logging.LogFactory

/**
 * TODO: Insert description here
 *
 * @author adamos
 */
class CleanableHdfsInputStream(in: HdfsDataInputStream) extends FSDataInputStream(in) {
    private val log = LogFactory.getLog(classOf[CleanableHdfsInputStream])

    override def seek(desired: Long): Unit =
      try {
        super.seek(desired)
      } catch {
        case e: IOException =>
          IOUtils.cleanup(log, in)
          throw e
      }
}
