/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_IPFS_IPFS_FALLBACK_REDIRECT_NAV_DATA_H
#define BRAVE_BROWSER_IPFS_IPFS_FALLBACK_REDIRECT_NAV_DATA_H

#include "base/supports_user_data.h"
#include "content/public/browser/navigation_controller.h"
#include "content/public/browser/navigation_entry.h"
#include "url/gurl.h"

namespace ipfs {

class IpfsFallbackRedirectNavigationData : public base::SupportsUserData::Data {

public:
    IpfsFallbackRedirectNavigationData() = default;
    explicit IpfsFallbackRedirectNavigationData(const GURL& url);
  //IpfsFallbackRedirectNavigationData(const IpfsFallbackRedirectNavigationData& ifrnd) : original_url_(ifrnd.original_url_) {}
    ~IpfsFallbackRedirectNavigationData() override;

    static IpfsFallbackRedirectNavigationData* GetOrCreateNavData(content::NavigationEntry* entry);
    static IpfsFallbackRedirectNavigationData* GetIpfsFallbackNavData(content::NavigationEntry* entry);
    static IpfsFallbackRedirectNavigationData* GetIpfsFallbackNavDataFromRedirectChain(content::NavigationController& controller);
    static void CloneUserDataToTheCurrentRedirectChainItem(content::NavigationController& controller);
    static bool IsAutoRedirectBlocked(content::NavigationHandle* handle, const GURL& current_page_url, const bool remove_from_history);

    GURL GetOriginalUrl() const;
    bool IsAutoRedirectBlocked() const;
    bool IsValid() const;

    std::string ToDebugString() const;

    std::unique_ptr<base::SupportsUserData::Data> Clone() override;

    void SetOriginalUrl(const GURL& url);
    void SetBlockAutoRedirect(const bool new_val);
    void SetValid(const bool new_val);
private:
  GURL original_url_;
  bool block_auto_redirect_{false};
  bool valid_{true};
};

}  // namespace ipfs

#endif  // BRAVE_BROWSER_IPFS_IPFS_FALLBACK_REDIRECT_NAV_DATA_H