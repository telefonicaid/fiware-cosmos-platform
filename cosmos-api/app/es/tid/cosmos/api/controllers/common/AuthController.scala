package es.tid.cosmos.api.controllers.common

import play.api.Play.current
import play.api.db.DB
import play.api.libs.json.Json
import play.api.mvc._

import es.tid.cosmos.api.authorization.ApiCredentials
import es.tid.cosmos.api.controllers.pages.CosmosProfile
import es.tid.cosmos.api.profile.CosmosProfileDao

/**
 * Controller able to check authentication and authorization
 */
trait AuthController extends Controller {
  private val AuthErrorResponse =
    Unauthorized(Json.toJson(Message("Invalid or missing Authorization header")))

  def Authenticated(request: Request[Any])(userAction: CosmosProfile => Result): Result = {
    (for {
      BasicAuth(apiKey, apiSecret) <- request.headers.get("Authorization")
      cosmosId <- DB.withConnection { implicit c =>
        CosmosProfileDao.lookupByApiCredentials(ApiCredentials(apiKey, apiSecret))
      }
    } yield {
      userAction(cosmosId)
    }).getOrElse(AuthErrorResponse)
  }
}
