package es.tid.cosmos.api

import play.api.Play
import play.api.Play.current
import play.api.libs.json._
import play.api.libs.json.JsString


/**
 * @author sortega
 */
package object controllers {

  /**
   * Represents a simple text message to the client.
   * @param m Text of the message
   */
  case class Message(m: String)

  implicit object MessageWrites extends Writes[Message]{
    def writes(instance: Message): JsValue = Json.obj(
      "message" -> JsString(instance.m)
    )
  }

  /**
   * Formats an internal server exception to JSON.
   * Stack traces are not shown in production mode.
   *
   * @param message One line explanation
   * @param ex      Cause of the internal error
   * @return        Error description
   */
  def formatInternalException(message: String, ex: Throwable): JsValue = {
    val basicMessage = Map("error" -> "%s: %s".format(message, ex.getMessage))
    val errorMessage = if (Play.isProd) basicMessage
    else basicMessage ++ Map("exception" -> ex.getClass.getCanonicalName,
                             "stack_trace" -> ex.getStackTraceString)
    Json.toJson(errorMessage)
  }

  def formatJsError(jsError: JsError): JsValue = {
    val formattedErrors = jsError.errors.toList.map {
      case (path, errors) => (path.toString(), errors.map(_.message).toList)
    }
    Json.toJson(Map(formattedErrors: _*))
  }
}
