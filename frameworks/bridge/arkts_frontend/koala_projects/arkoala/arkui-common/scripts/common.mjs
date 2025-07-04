/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import path from "path"
import { ohConf } from "../../ohos-sdk/scripts/ohconf.mjs"
import { fileURLToPath } from "url"

export const SCRIPTS_DIR = path.dirname(fileURLToPath(import.meta.url))
export const OHCONF_JSON = path.join(SCRIPTS_DIR, "../.ohconf.json")

export const OHSDK_VERSION = ohConf(OHCONF_JSON).sdkVersion()
export const OHSDK = ohConf(OHCONF_JSON).value("ohos_sdk_path", true)
