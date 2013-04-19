package es.tid.cosmos.api.controllers

import play.api.mvc._

trait Pages extends Controller {

  def index = Action {
    Ok(views.html.index("Your new application is ready."))
  }
}