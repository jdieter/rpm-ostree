/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*-
 *
 * Copyright (C) 2014 Colin Walters <walters@verbum.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2 of the licence or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#pragma once

#include <gio/gio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ostree.h>

#define _N(single, plural, n) ( (n) == 1 ? (single) : (plural) )
#define _NS(n) _N("", "s", n)

int
rpmostree_ptrarray_sort_compare_strings (gconstpointer ap,
                                         gconstpointer bp);

GVariant *_rpmostree_vardict_lookup_value_required (GVariantDict *dict,
                                                    const char *key,
                                                    const GVariantType *fmt,
                                                    GError     **error);

char *
_rpmostree_varsubst_string (const char *instr,
                            GHashTable *substitutions,
                            GError **error);

gboolean
_rpmostree_util_update_checksum_from_file (GChecksum    *checksum,
                                           int           rootfs_dfd,
                                           const char   *path,
                                           GCancellable *cancellable,
                                           GError      **error);

GPtrArray *
_rpmostree_util_get_commit_hashes (OstreeRepo *repo,
                                   const char *beg,
                                   const char *end,
                                   GCancellable *cancellable,
                                   GError **error);

char *
_rpmostree_util_next_version (const char *auto_version_prefix,
                              const char *last_version);

char *
rpmostree_str_replace (const char  *buf,
                       const char  *old,
                       const char  *new,
                       GError     **error);


gboolean
rpmostree_pull_content_only (OstreeRepo  *dest,
                             OstreeRepo  *src,
                             const char  *src_commit,
                             GCancellable *cancellable,
                             GError      **error);
const char *
rpmostree_file_get_path_cached (GFile *file);

static inline
const char *
gs_file_get_path_cached (GFile *file)
{
  return rpmostree_file_get_path_cached (file);
}

void rpmostree_diff_print (OstreeRepo *repo,
                           GPtrArray *removed,
                           GPtrArray *added,
                           GPtrArray *modified_old,
                           GPtrArray *modified_new);

gboolean
rpmostree_str_has_prefix_in_strv (const char *str,
                                  char      **strv,
                                  int         n);

gboolean
rpmostree_str_has_prefix_in_ptrarray (const char *str,
                                      GPtrArray  *prefixes);

gboolean
rpmostree_str_ptrarray_contains (GPtrArray  *strs,
                                 const char *str);

gboolean
rpmostree_deployment_get_layered_info (OstreeRepo        *repo,
                                       OstreeDeployment  *deployment,
                                       gboolean          *out_is_layered,
                                       char             **out_base_layer,
                                       char            ***out_layered_pkgs,
                                       GVariant         **out_removed_base_pkgs,
                                       GVariant         **out_replaced_base_pkgs,
                                       GError           **error);

gboolean
rpmostree_get_pkgcache_repo (OstreeRepo   *parent,
                             OstreeRepo  **out_pkgcache,
                             GCancellable *cancellable,
                             GError      **error);

gboolean
rpmostree_decompose_sha256_nevra (const char **nevra,
                                  char       **sha256,
                                  GError     **error);

char *
rpmostree_cache_branch_to_nevra (const char *cachebranch);

/* https://github.com/ostreedev/ostree/pull/1132 */
typedef OstreeRepo _OstreeRepoAutoTransaction;
static inline void
_ostree_repo_auto_transaction_cleanup (void *p)
{
  OstreeRepo *repo = p;
  if (repo)
    (void) ostree_repo_abort_transaction (repo, NULL, NULL);
}

static inline _OstreeRepoAutoTransaction *
_ostree_repo_auto_transaction_start (OstreeRepo     *repo,
                                     GCancellable   *cancellable,
                                     GError        **error)
{
  if (!ostree_repo_prepare_transaction (repo, NULL, cancellable, error))
    return NULL;
  return (_OstreeRepoAutoTransaction *)repo;
}
G_DEFINE_AUTOPTR_CLEANUP_FUNC (_OstreeRepoAutoTransaction, _ostree_repo_auto_transaction_cleanup)
