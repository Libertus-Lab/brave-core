// Copyright (c) 2023 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef BRAVE_COMPONENTS_PSST_BROWSER_CORE_PSST_RULE_SERVICE_H_
#define BRAVE_COMPONENTS_PSST_BROWSER_CORE_PSST_RULE_SERVICE_H_

#include <memory>
#include <string>
#include <vector>

#include "base/component_export.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/singleton.h"
#include "base/memory/weak_ptr.h"
#include "brave/components/psst/browser/core/psst_rule.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

class GURL;

namespace psst {

class COMPONENT_EXPORT(PSST_BROWSER_CORE) PsstRuleService {
 public:
  PsstRuleService(const PsstRuleService&) = delete;
  PsstRuleService& operator=(const PsstRuleService&) = delete;
  ~PsstRuleService();
  COMPONENT_EXPORT(PSST_BROWSER_CORE)
  void CheckIfMatch(const GURL& url,
                    base::OnceCallback<void(MatchedRule)> cb) const;
  COMPONENT_EXPORT(PSST_BROWSER_CORE)
  static PsstRuleService* GetInstance();  // singleton
  void LoadPsstRules(const base::FilePath& path);

 protected:
  PsstRuleService();

 private:
  void OnFileDataReady(const std::string& data);
  base::FilePath component_path_;
  std::vector<std::unique_ptr<PsstRule>> rules_;

  base::WeakPtrFactory<PsstRuleService> weak_factory_{this};

  friend struct base::DefaultSingletonTraits<PsstRuleService>;
};

}  // namespace psst

#endif  // BRAVE_COMPONENTS_PSST_BROWSER_CORE_PSST_RULE_SERVICE_H_
