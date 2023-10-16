/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */
 
 #include "brave/browser/ipfs/ipfs_fallback_redirect_nav_data.h"

#include <memory>
#include <utility>

#include "base/strings/stringprintf.h"
#include "brave/components/ipfs/ipfs_utils.h"
#include "content/public/browser/navigation_handle.h"
#include "content/public/browser/navigation_controller.h"
#include "content/public/browser/web_contents.h"

namespace {
    const char kIpfsFallbackRedirectNavigationDataKey[] = "pfs-fallback-redirect-nav-data";

}

namespace ipfs {

IpfsFallbackRedirectNavigationData::IpfsFallbackRedirectNavigationData(const GURL& url): original_url_(url) {}

IpfsFallbackRedirectNavigationData::~IpfsFallbackRedirectNavigationData() = default;


// static
IpfsFallbackRedirectNavigationData* IpfsFallbackRedirectNavigationData::GetOrCreateNavData(content::NavigationEntry* entry) {
    DCHECK(entry);

    auto* data = static_cast<IpfsFallbackRedirectNavigationData*>(
        entry->GetUserData(kIpfsFallbackRedirectNavigationDataKey));

    if(!data) {
      auto nav_data = std::make_unique<IpfsFallbackRedirectNavigationData>();
      data = nav_data.get();
      entry->SetUserData(kIpfsFallbackRedirectNavigationDataKey, std::move(nav_data));    
    } 

    return data;
}

// static
IpfsFallbackRedirectNavigationData* IpfsFallbackRedirectNavigationData::GetIpfsFallbackNavData(content::NavigationEntry* entry) {
    DCHECK(entry);
    auto* data = static_cast<IpfsFallbackRedirectNavigationData*>(
        entry->GetUserData(kIpfsFallbackRedirectNavigationDataKey));
    return data;
}

// static
IpfsFallbackRedirectNavigationData* IpfsFallbackRedirectNavigationData::GetIpfsFallbackNavDataFromRedirectChain(content::NavigationController& controller) {
    IpfsFallbackRedirectNavigationData* ipfs_fallback_nav_data = nullptr; 
    for(int i = 0; i < controller.GetEntryCount(); i++) {
      auto* entry = controller.GetEntryAtIndex(i);
      if(!entry) {
        continue;
      }
      if(!ipfs_fallback_nav_data) {
        auto* nav_data = GetIpfsFallbackNavData(entry);
        ipfs_fallback_nav_data = (nav_data && nav_data->IsValid()) ? nav_data : nullptr;
      } else {
        break;
      }
    }

    return ipfs_fallback_nav_data;
}

// static
void IpfsFallbackRedirectNavigationData::CloneUserDataToTheCurrentRedirectChainItem(content::NavigationController& controller) {
    IpfsFallbackRedirectNavigationData* ipfs_fallback_nav_data = nullptr; 
    for(int i = 0; i < controller.GetEntryCount(); i++) {
      auto* entry = controller.GetEntryAtIndex(i);
      if(!entry) {
        continue;
      } 
      if(!ipfs_fallback_nav_data) {
        auto* nav_data = GetIpfsFallbackNavData(entry);
        ipfs_fallback_nav_data = (nav_data && nav_data->IsValid()) ? nav_data : nullptr;
          LOG(INFO) << "[IPFS] CloneUserDataToTheCurrentRedirectChainItem found first chain"
          << "\r\nEntryID:" << entry->GetUniqueID()
          << "\r\nEntryUrl:" << entry->GetURL()
          << "\r\nipfs_fallback_nav_data:" << (ipfs_fallback_nav_data ? ipfs_fallback_nav_data->ToDebugString() : "n/a")
          ;
      } else if(ipfs::ExtractSourceFromGateway(ipfs_fallback_nav_data->GetOriginalUrl()) == ipfs::ExtractSourceFromGateway(entry->GetURL())) {
        LOG(INFO) << "[IPFS] CloneUserDataToTheCurrentRedirectChainItem copied fallback data to"
          << "\r\nEntryID:" << entry->GetUniqueID()
          << "\r\nEntryUrl:" << entry->GetURL()
          << "\r\nipfs_fallback_nav_data:" << (ipfs_fallback_nav_data ? ipfs_fallback_nav_data->ToDebugString() : "n/a")
        ;
        auto* nav_data = GetOrCreateNavData(entry);// TODO use clome somehow
        nav_data->SetOriginalUrl(ipfs_fallback_nav_data->GetOriginalUrl());
        nav_data->SetBlockAutoRedirect(ipfs_fallback_nav_data->IsAutoRedirectBlocked());
        nav_data->SetValid(ipfs_fallback_nav_data->IsValid());
        ipfs_fallback_nav_data->SetValid(false);
      } else {
        LOG(INFO) << "[IPFS] CloneUserDataToTheCurrentRedirectChainItem chain broken"
          << "\r\nEntryID:" << entry->GetUniqueID()
          << "\r\nEntryUrl:" << entry->GetURL()
          << "\r\nipfs_fallback_nav_data:" << (ipfs_fallback_nav_data ? ipfs_fallback_nav_data->ToDebugString() : "n/a")
          << "\r\nipfs from original:" << (ipfs::ExtractSourceFromGateway(ipfs_fallback_nav_data->GetOriginalUrl())).value()
          << "\r\nipfs from entryUrl:" << (ipfs::ExtractSourceFromGateway(entry->GetURL())).value()
        ;
      }
    }
}

// static
bool IpfsFallbackRedirectNavigationData::IsAutoRedirectBlocked(content::NavigationHandle* handle, const GURL& current_page_url, const bool remove_from_history) {
    bool is_blocked{false};
              LOG(INFO) << "[IPFS] IsAutoRedirectBlocked #0, contr:" << (handle->GetWebContents()->GetController().GetEntryCount());
    auto& controller = handle->GetWebContents()->GetController();
    for(int i = 0; i < controller.GetEntryCount(); i++) {
      auto* entry = controller.GetEntryAtIndex(i);
      if(!entry) {
        continue;
      }    
      const auto* nav_data = GetIpfsFallbackNavData(entry);
      is_blocked = nav_data && nav_data->IsValid() && nav_data->IsAutoRedirectBlocked() && !nav_data->GetOriginalUrl().is_empty() && nav_data->GetOriginalUrl() == current_page_url;
      if(is_blocked) {
        if(remove_from_history) {
          controller.RemoveEntryAtIndex(i);
        }
        break;
      }
    }
    return is_blocked;
}

GURL IpfsFallbackRedirectNavigationData::GetOriginalUrl() const {
    return original_url_;
}

bool IpfsFallbackRedirectNavigationData::IsAutoRedirectBlocked() const {
    return block_auto_redirect_;
}

bool IpfsFallbackRedirectNavigationData::IsValid() const {
    return valid_;
}

std::string IpfsFallbackRedirectNavigationData::ToDebugString() const {
    return base::StringPrintf("valid:%d block_auto_redirect:%d original_url:%s", valid_, block_auto_redirect_, original_url_.spec().c_str());
}

std::unique_ptr<base::SupportsUserData::Data> IpfsFallbackRedirectNavigationData::Clone() {
    auto copy = std::make_unique<IpfsFallbackRedirectNavigationData>();
    copy->original_url_ = original_url_;
    copy->block_auto_redirect_ = block_auto_redirect_;
    copy->valid_ = valid_;
    return std::move(copy);
}

void IpfsFallbackRedirectNavigationData::SetOriginalUrl(const GURL& url) {
    original_url_ = url;
}

void IpfsFallbackRedirectNavigationData::SetBlockAutoRedirect(const bool new_val) {
    block_auto_redirect_ = new_val;
}

void IpfsFallbackRedirectNavigationData::SetValid(const bool new_val) {
    valid_ = new_val;
}

}  // namespace ipfs