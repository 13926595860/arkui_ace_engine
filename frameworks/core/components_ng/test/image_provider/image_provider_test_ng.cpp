/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "gtest/gtest.h"

#define protected public
#define private public

#include "core/components_ng/image_provider/image_loading_context.h"
#include "core/components_ng/image_provider/image_provider.h"
#include "core/components_ng/image_provider/image_state_manager.h"
#include "core/components_ng/image_provider/pixel_map_image_object.h"
#include "core/components_ng/test/mock/image_provider/mock_image_loader.h"
#include "core/pipeline_ng/test/mock/mock_pipeline_base.h"
#include "test/mock/base/mock_pixel_map.h"

using namespace testing;
using namespace testing::ext;

namespace {
const char* SRC_JPG = "file://data/data/com.example.test/res/exampleAlt.jpg";
const char* SRC_THUMBNAIL = "datashare:///media/9/thumbnail/300/300";
constexpr int32_t LENGTH_100 = 100;
} // namespace
namespace OHOS::Ace::NG {
class ImageProviderTestNg : public testing::Test {
public:
    static void SetUpTestSuite();
    static void TearDownTestSuite();
    static void WaitForAsyncTasks();
};

void ImageProviderTestNg::SetUpTestSuite()
{
    MockPipelineBase::SetUp();
    g_loader = AceType::MakeRefPtr<MockImageLoader>();
    g_threads = std::vector<std::thread>();
}

void ImageProviderTestNg::TearDownTestSuite()
{
    MockPipelineBase::TearDown();
    g_loader = nullptr;
}

// wait for load task to finish
void ImageProviderTestNg::WaitForAsyncTasks()
{
    for (auto& thread : g_threads) {
        thread.join();
    }
    g_threads.clear();
}

/**
 * @tc.name: ImageProviderTestNg001
 * @tc.desc: Test ImageProvider Synchronous CreateImageObj failure
 * @tc.type: FUNC
 */
HWTEST_F(ImageProviderTestNg, ImageProviderTestNg001, TestSize.Level1)
{
    EXPECT_CALL(*g_loader, LoadImageData).Times(1);
    auto src = ImageSourceInfo(SRC_JPG);
    auto ctx = AceType::MakeRefPtr<ImageLoadingContext>(src, LoadNotifier(nullptr, nullptr, nullptr), true);
    EXPECT_EQ(ctx->stateManager_->GetCurrentState(), ImageLoadingState::UNLOADED);
    ctx->LoadImageData();

    EXPECT_EQ(ctx->syncLoad_, true);
    EXPECT_EQ(ctx->imageObj_, nullptr);
    EXPECT_EQ(ctx->stateManager_->GetCurrentState(), ImageLoadingState::LOAD_FAIL);
}

/**
 * @tc.name: ImageProviderTestNg002
 * @tc.desc: Test ImageProvider merging repeated tasks
 * @tc.type: FUNC
 */
HWTEST_F(ImageProviderTestNg, ImageProviderTestNg002, TestSize.Level1)
{
    EXPECT_CALL(*g_loader, LoadImageData).Times(1);
    auto src = ImageSourceInfo(SRC_JPG);
    // create 20 repeated tasks
    std::vector<RefPtr<ImageLoadingContext>> contexts(20);
    for (auto& ctx : contexts) {
        ctx = AceType::MakeRefPtr<ImageLoadingContext>(src, LoadNotifier(nullptr, nullptr, nullptr), false);
        ctx->LoadImageData();
    }
    // check task map
    {
        std::scoped_lock<std::mutex> lock(ImageProvider::taskMtx_);
        EXPECT_EQ(ImageProvider::tasks_.size(), (size_t)1);
        EXPECT_EQ(ImageProvider::tasks_[src.GetKey()].ctxs_.size(), (size_t)20);
    }
    // wait for load task to finish
    WaitForAsyncTasks();
    std::scoped_lock<std::mutex> lock(ImageProvider::taskMtx_);
    EXPECT_EQ(ImageProvider::tasks_.size(), (size_t)0);
}

/**
 * @tc.name: ImageProviderTestNg003
 * @tc.desc: Test ImageProvider load thumbnail
 * @tc.type: FUNC
 */
HWTEST_F(ImageProviderTestNg, ImageProviderTestNg003, TestSize.Level1)
{
    auto src = ImageSourceInfo(SRC_THUMBNAIL);

    EXPECT_CALL(*g_loader, LoadDecodedImageData).Times(1);
    auto ctx = AceType::MakeRefPtr<ImageLoadingContext>(src, LoadNotifier(nullptr, nullptr, nullptr), true);
    ctx->LoadImageData();
}

/**
 * @tc.name: ImageProviderTestNg004
 * @tc.desc: Test ImageProvider load success
 * @tc.type: FUNC
 */
HWTEST_F(ImageProviderTestNg, ImageProviderTestNg004, TestSize.Level1)
{
    auto src = ImageSourceInfo(SRC_THUMBNAIL);

    EXPECT_CALL(*g_loader, LoadDecodedImageData).Times(1);
    auto ctx = AceType::MakeRefPtr<ImageLoadingContext>(src, LoadNotifier(nullptr, nullptr, nullptr));
    ctx->LoadImageData();
    EXPECT_EQ(ctx->stateManager_->state_, ImageLoadingState::DATA_LOADING);

    WaitForAsyncTasks();

    // mock data loading succeeded
    ctx->stateManager_->state_ = ImageLoadingState::DATA_LOADING;
    auto pixmap = AceType::MakeRefPtr<MockPixelMap>();
    SizeF size(LENGTH_100, LENGTH_100);
    ctx->DataReadyCallback(AceType::MakeRefPtr<PixelMapImageObject>(pixmap, src, size));
    EXPECT_EQ(ctx->stateManager_->state_, ImageLoadingState::DATA_READY);
    EXPECT_TRUE(ctx->NeedAlt());
    EXPECT_EQ(ctx->GetImageSize(), size);
}
} // namespace OHOS::Ace::NG
