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

package es.tid.cosmos.api.controllers.pages

import es.tid.cosmos.api.authorization.ApiCredentials
import es.tid.cosmos.api.profile.Quota

/**
 * Represents the Cosmos-specific user profile.
 *
 * @param id             Internal user id
 * @param handle         User login
 * @param quota          Quota for resources
 * @param apiCredentials Credentials for the REST API
 * @param keys           Public keys
 */
case class CosmosProfile(
    id: Long,
    handle: String,
    quota: Quota,
    apiCredentials: ApiCredentials,
    keys: NamedKey*)

/**
 * A public key annotated with a user-defined name.
 *
 * @param name       Key name
 * @param signature  Public key signature
 */
case class NamedKey(name: String, signature: String)

/**
 * Represents a new user registration.
 *
 * @param handle    Handle name to be used as login on clusters
 * @param publicKey Initial public key
 */
case class Registration(handle: String, publicKey: String)
