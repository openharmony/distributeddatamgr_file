/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

/**
 * @Syscap SysCap.ACE.UIEngine
 */
export default class FileShare {
  /**
   * Convert uri to share with given arguments.
   * @param options Options.
   */
  static fuzzyFileToUri(options: {
    /**
     * URI of the file.
     */
    uri: string;

    /**
     * Permission identifier.
     */
    authority: string;
    
    /**
     * Serial number of external equipment.
     */
    deviceId: string;
    
    /**
     * Name to display.
     */
    displayName?: string;

    /**
     * Called when the uri is converted successfully.
     * This function returns the URI of the new uri to share.
     */
    success?: (uri: string) => void;

    /**
     * Called when converting fails.
     */
    fail?: (data: any, code: number) => void;

    /**
     * Called when the execution is completed.
     */
    complete?: () => void;
  }): void;
}
