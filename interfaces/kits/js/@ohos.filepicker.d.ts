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

export interface FileResponse {
  /**
   * File URI.
   */
  uri: string;

  /**
   * File or directory name.
   */
  name: string;
  
  /**
   * File size, in bytes.
   */
  length: number;
  
  /**
   * Timestamp when the file is stored, which is the number of milliseconds elapsed since 1970/01/01 00:00:00 GMT.
   */
  lastModifiedTime: number;

  /**
   * File type. Available values are as follows:
   * dir: directory
   * file: file
   */
  type: "dir" | "file";

  /**
   * The number of file or directory in the folder when the param type is 'dir'.
   */
  num: number;

  /**
   * File suffix.
   */
  suffix: string;
}

export interface DeviceResponse {
  /**
   * File URI.
   */
  uri: string;

  /**
   * Serial number of external equipment.
   */
  deviceID: string;

  /**
   * External equipment mounting point.
   */
  mountID: number;

  /**
   * External equipment ID.
   */
  diskID: string;
}

export interface SaveResponse {
  /**
   * Whether the file to copy is success.
   */
  status: boolean;

  /**
   * URI of the file to copy.
   */
  srcUri: string;

  /**
   * URI of the location to which the copy is to save when the param status is true.
   */
  dstUri: string;
}

/**
 * @Syscap SysCap.ACE.UIEngine
 */
export default class FilePicker {
  /**
   * Creates a specified directory.
   * @param options Options.
   */
  static mkdir(options: {
    /**
     * URI of the directory.
     */
    uri: string;

    /**
     * Called when the directory is created successfully.
     */
    success?: () => void;

    /**
     * Called when the creation fails.
     */
    fail?: (data: any, code: number) => void;

    /**
     * Called when the execution is completed.
     */
    complete?: () => void;
  }): void;

  /**
   * Save a array of files to a specified location.
   * @param options Options.
   */
  static saveFile(options: {
    /**
     * Array of files' URI to save
     */
    srcUri: Array<string>;

    /**
     * URI of the location to which the saveFile is to save.
     * The directory of application resources and URI of the tmp type are not supported.
     */
    dstUri: string;

    /**
     * Called when the source file is saved to the specified location successfully.
     * This function returns the saveList of the files preservation.
     */
    success?: (data: { saveList: Array<SaveResponse> }) => void;

    /**
     * Called when the copy or save operation fails.
     */
    fail?: (data: any, code: number) => void;

    /**
     * Called when the execution is completed.
     */
    complete?: () => void;
  }): void;

  /**
   * Obtains the list of external devices information.
   * @param options Options.
   */
  static getDeviceInfo(options: {
    /**
     * Called when the external devices information is obtained successfully.
     */
    success?: (data: { deviceList: Array<DeviceResponse> }) => void;

    /**
     * Called when the external devices information fails to be obtained.
     */
    fail?: (data: any, code: number) => void;

    /**
     * Called when the execution is completed.
     */
    complete?: () => void;
  }): void;
  
  /**
   * Obtains the list of files in a specified directory.
   * @param options Options.
   */
  static list(options: {
    /**
     * URI of the directory.
     */
    uri: string;

    /**
     * Called when the list is obtained successfully.
     */
    success?: (data: { fileList: Array<FileResponse> }) => void;

    /**
     * Called when the list fails to be obtained.
     */
    fail?: (data: any, code: number) => void;

    /**
     * Called when the execution is completed.
     */
    complete?: () => void;
  }): void;

  /**
   * Search for files or folders name containing keyword in a path recursively.
   * @param options
   */
  static searchFile(options: {
    /**
     * Keywords to search.
     */
    name: string;

    /**
     * Uri to search.
     */
    uri: string;

    /**
     * Called when search successfully.
     */
    success?: (data: { fileList: Array<FileResponse> }) => void;

    /**
     * Called when search faily.
     */
    fail?: (data: any, code: number) => void;

    /**
     * Called when the execution is completed.
     */
    complete?: () => void;
  }): void;
}
