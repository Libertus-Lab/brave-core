/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "chrome/test/base/testing_profile.h"

#include "brave/browser/search_engines/search_engine_tracker.h"
#include "brave/browser/ui/commands/accelerator_service_factory.h"
#include "brave/components/brave_ads/browser/ads_service.h"
#include "brave/components/brave_rewards/browser/rewards_service.h"
#include "chrome/browser/prefs/browser_prefs.h"
#include "components/pref_registry/pref_registry_syncable.h"

#define RegisterUserProfilePrefs(REGISTRY) \
  RegisterBraveUserProfilePrefs(REGISTRY); \
  RegisterUserProfilePrefs(REGISTRY)

#define ChromeBrowsingDataRemoverDelegate BraveBrowsingDataRemoverDelegate
#include "src/chrome/test/base/testing_profile.cc"
#undef ChromeBrowsingDataRemoverDelegate
#undef RegisterUserProfilePrefs

void TestingProfile::RegisterBraveUserProfilePrefs(
    user_prefs::PrefRegistrySyncable* registry) {
  // Register prefs for services that in a regular profile would get registered
  // by the DependencyManager.
  brave_rewards::RewardsService::RegisterProfilePrefs(registry);
  brave_ads::AdsService::RegisterProfilePrefs(registry);
  commands::AcceleratorServiceFactory::GetInstance();
  SearchEngineTrackerFactory::GetInstance();
}

#include "brave/test/base/brave_testing_profile.cc"
