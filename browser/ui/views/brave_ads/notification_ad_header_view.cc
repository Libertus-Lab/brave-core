/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/brave_ads/notification_ad_header_view.h"

#include "base/check.h"
#include "brave/browser/ui/views/brave_ads/insets_util.h"
#include "brave/browser/ui/views/brave_ads/spacer_view.h"
#include "ui/base/metadata/metadata_impl_macros.h"
#include "ui/gfx/font_list.h"
#include "ui/gfx/geometry/insets.h"
#include "ui/gfx/geometry/size.h"
#include "ui/native_theme/native_theme.h"
#include "ui/views/accessibility/view_accessibility.h"
#include "ui/views/border.h"
#include "ui/views/controls/label.h"
#include "ui/views/layout/flex_layout.h"

namespace brave_ads {

namespace {

// Spacing around each child view
constexpr auto kMargin = gfx::Insets::TLBR(0, 0, 0, 0);

// Spacing between child views and host views
constexpr auto kInteriorMargin = gfx::Insets::TLBR(0, 10, 0, 0);

constexpr int kHeaderViewHeight = 22;

constexpr int kControlButtonsSpacing = 10;

constexpr char kTitleFontName[] = "Roboto";
constexpr gfx::Font::FontStyle kTitleFontStyle = gfx::Font::NORMAL;
constexpr int kTitleFontSize = 13;
constexpr gfx::Font::Weight kTitleFontWeight = gfx::Font::Weight::MEDIUM;
constexpr SkColor kLightModeTitleColor = SkColorSetRGB(0x00, 0x00, 0x00);
constexpr SkColor kDarkModeTitleColor = SkColorSetRGB(0xe3, 0xe6, 0xec);

constexpr gfx::HorizontalAlignment kTitleHorizontalAlignment = gfx::ALIGN_LEFT;
constexpr gfx::VerticalAlignment kTitleVerticalAlignment = gfx::ALIGN_BOTTOM;

constexpr auto kTitleBorderInsets = gfx::Insets::TLBR(0, 10, 3, 0);

}  // namespace

NotificationAdHeaderView::NotificationAdHeaderView() {
  CreateView();

  GetViewAccessibility().SetRole(ax::mojom::Role::kGenericContainer);
}

NotificationAdHeaderView::~NotificationAdHeaderView() = default;

void NotificationAdHeaderView::SetTitle(const std::u16string& text) {
  CHECK(title_label_);
  title_label_->SetText(text);
  UpdateAccessibleName();
}

void NotificationAdHeaderView::SetTitleElideBehavior(
    gfx::ElideBehavior elide_behavior) {
  CHECK(title_label_);
  title_label_->SetElideBehavior(elide_behavior);
}

void NotificationAdHeaderView::UpdateContent() {
  UpdateTitleLabel();
}

void NotificationAdHeaderView::OnThemeChanged() {
  View::OnThemeChanged();

  UpdateContent();
}

///////////////////////////////////////////////////////////////////////////////

void NotificationAdHeaderView::CreateView() {
  views::FlexLayout* layout_manager =
      SetLayoutManager(std::make_unique<views::FlexLayout>());
  layout_manager->SetDefault(views::kMarginsKey, kMargin);
  layout_manager->SetInteriorMargin(kInteriorMargin);
  layout_manager->SetCollapseMargins(true);

  CHECK(!title_label_);
  title_label_ = AddChildView(CreateTitleLabel());

  views::View* control_button_spacing_view =
      CreateFixedSizeSpacerView(kControlButtonsSpacing);
  AddChildView(control_button_spacing_view);

  const gfx::Size size(GetPreferredSize().width(), kHeaderViewHeight);
  SetPreferredSize(size);

  // Not focusable by default, only for accessibility
  SetFocusBehavior(FocusBehavior::ACCESSIBLE_ONLY);
}

std::unique_ptr<views::Label> NotificationAdHeaderView::CreateTitleLabel() {
  auto label = std::make_unique<views::Label>();

  const gfx::FontList font_list({kTitleFontName}, kTitleFontStyle,
                                kTitleFontSize, kTitleFontWeight);
  label->SetFontList(font_list);

  label->SetBackgroundColor(SK_ColorTRANSPARENT);

  label->SetHorizontalAlignment(kTitleHorizontalAlignment);
  label->SetVerticalAlignment(kTitleVerticalAlignment);

  const int line_height = font_list.GetHeight();
  label->SetLineHeight(line_height);

  gfx::Insets border_insets = kTitleBorderInsets;
  AdjustInsetsForFontList(&border_insets, font_list);
  label->SetBorder(views::CreateEmptyBorder(border_insets));

  const views::FlexSpecification flex_specification =
      views::FlexSpecification(views::MinimumFlexSizeRule::kScaleToZero,
                               views::MaximumFlexSizeRule::kUnbounded);
  label->SetProperty(views::kFlexBehaviorKey, flex_specification);

  label->SetHandlesTooltips(false);

  return label;
}

void NotificationAdHeaderView::UpdateTitleLabel() {
  const bool should_use_dark_colors = GetNativeTheme()->ShouldUseDarkColors();

  CHECK(title_label_);
  title_label_->SetEnabledColor(should_use_dark_colors ? kDarkModeTitleColor
                                                       : kLightModeTitleColor);
}

void NotificationAdHeaderView::UpdateAccessibleName() {
  GetViewAccessibility().SetName(std::u16string(title_label_->GetText()));
}

BEGIN_METADATA(NotificationAdHeaderView)
END_METADATA

}  // namespace brave_ads
