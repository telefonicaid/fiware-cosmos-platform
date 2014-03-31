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

package es.tid.cosmos.infinity.server.authorization

import scalaz.{Scalaz, Validation}

import es.tid.cosmos.infinity.server.actions.Action
import es.tid.cosmos.infinity.server.actions.Action._
import es.tid.cosmos.infinity.server.authentication.UserProfile
import es.tid.cosmos.infinity.server.authorization.AuthorizationProvider._
import es.tid.cosmos.infinity.server.db.InfinityDataStore
import es.tid.cosmos.infinity.server.fs._
import es.tid.cosmos.infinity.server.util.Path

private[authorization] class PersistentAuthorization(store: InfinityDataStore) {
  import Scalaz._

  def authorize(action: Action, profile: UserProfile): AuthorizationResponse =
    store.withTransaction { implicit c =>
      val path = action.on

      def requireValidPath(path: String) =
        Validation.fromTryCatch(Path.absolute(path)).leftMap(_ => permissionDenied)

      def requireValidDestination(destination: String) = for {
        destinationPath <- requireValidPath(destination)
        parentPath <- destinationPath.parent.toSuccess(permissionDenied)
        parentInode <- requireAccessibleInode(parentPath)
        _ <- requireUserCanWrite(parentInode)
      } yield destinationPath

      def requireAccessibleParentInode() = for {
        parentPath <- path.parent.toSuccess(permissionDenied)
        inode <- requireAccessibleInode(parentPath)
      } yield inode

      def requireWritableParentInode() = requireAccessibleParentInode().flatMap(requireUserCanWrite)

      def requireAccessibleInode(path: Path = action.on) =
        store.inodeDao.load(path, profile).leftMap(_ => permissionDenied)

      def deepestCreatedInode(path: Path = action.on): Inode =
        store.inodeDao.load(path, profile).fold(
          fail = _ => deepestCreatedInode(path.parent.get),
          succ = identity
        )

      def requireUserCanRead(inode: Inode) =
        if (inode.canRead(profile)) inode.success else permissionDenied.failure

      def requireUserCanWrite(inode: Inode) =
        if (inode.canWrite(profile)) inode.success else permissionDenied.failure

      def requireOwnedInode(inode: Inode) =
        if (inode.permissions.owner == profile.username) inode.success
        else permissionDenied.failure

      def permissionDenied = PermissionDenied(path)

      def validation: Validation[AccessException, _] = action match {
        case _: Open | _: GetFileStatus | _: GetFileChecksum | _: ListStatus |  _: GetContentSummary =>
          requireAccessibleInode().flatMap(requireUserCanRead)
        case _: GetHomeDirectory =>
          ().success
        case _: Create =>
          requireAccessibleInode().flatMap(requireUserCanWrite) orElse requireWritableParentInode()
        case _: Mkdirs =>
          requireUserCanWrite(deepestCreatedInode())
        case Rename(_, _, destination) =>
          for {
            _ <- requireWritableParentInode()
            _ <- requireAccessibleInode()
            _ <- requireValidDestination(destination)
          } yield ()
        case _: SetReplication | _: SetTimes | _: Append =>
          requireAccessibleInode().flatMap(requireUserCanWrite)
        case _: GetDelegationToken | _: RenewDelegationToken | _: CancelDelegationToken | _: SetOwner =>
          permissionDenied.failure
        case _: SetPermission =>
          for {
            _ <- requireWritableParentInode()
            _ <- requireAccessibleInode().flatMap(requireOwnedInode)
          } yield ()
        case _: Delete =>
          requireWritableParentInode()
      }

      if (profile.superuser) Authorized
      else validation.fold(fail = AuthorizationFailed.apply, succ = _ => Authorized)
    }
}
