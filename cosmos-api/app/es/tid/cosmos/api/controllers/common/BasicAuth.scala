package es.tid.cosmos.api.controllers.common

import scala.util.Try

import com.ning.http.util.Base64

/**
 * Extractor/formatter class for basic HTTP authorization headers.
 */
object BasicAuth {
  /**
   * Formats a realm and a password as a basic HTTP authorization header.
   */
  def apply(realm: String, password: String): String = {
    val encoded = new String(Base64.encode(s"$realm:$password".getBytes))
    s"Basic $encoded"
  }

  /**
   * Extracts a tuple of realm and password from a valid HTTP Auth Basic header.
   *
   * @param header String of the form "Basic: base-64-encoded-realm-pass"
   * @return       Realm and password or nothing
   */
  def unapply(header: String): Option[(String, String)] =
    if (header.toLowerCase.startsWith("basic ")) decodeCredentials(header.substring(6))
    else None

  private def decodeCredentials(encodedAuthorization: String) =
    decode(encodedAuthorization).map(_.split(':')).collect {
      case Array(realm, pass) => (realm, pass)
    }

  private def decode(s: String) = Try(Base64.decode(s)).toOption.map(new String(_))
}
