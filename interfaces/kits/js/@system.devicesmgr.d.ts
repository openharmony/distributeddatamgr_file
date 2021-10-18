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
export interface VolumeInfo {
   /**
    * 
    */
   mId: string;
 
   /**
    * 
    */
   mDiskId: string;
 
   /**
    * 
    */
   mPartGuid: string;

   /**
    * 
    */
   mFsUuid: string;
 
   /**
    * 
    */
   mType: number;

   /**
    * 
    */
   mMountFlags: number;
   
   /**
    * 
    */
   mMountUserId: number;

   /**
    * 
    */
   mState: number;

   /**
    * 
    */
   mPath: string;

   /**
    * 
    */
   mInternalPath: string;

    /**
    * 
    */
   mFsLabel: string;
}
export interface DiskInfo {
   /**
    * 
    */
   mId: string;

   /**
    * 
    */
   mSysPath: string;

   /**
    * 
    */
   mSize: number;

   /**
    * 
    */
   mLabel: string;

   /**
    * 
    */
   mFlags: number;
}

export default class DeviceSMgr {

   static mount(options: {
      /**
       *
       */
      volId: string;

      /**
       *
       */
      success?: () => void;

      /**
       *
       */
      fail?: (data: any, code: number) => void;

      /**
       * Called when the execution is completed.
       */
      complete?: () => void;

   }): void;

   static unMount(options: {
      /**
       *
       */
      volId: string;

      /**
       *
       */
      success?: () => void;

      /**
       *
       */
      fail?: (data: any, code: number) => void;

      /**
       * Called when the execution is completed.
       */
      complete?: () => void;

   }): void;

   static format(options: {
      /**
       *
       */
      volId: string;

      /**
       *
       */
      success?: () => void;

      /**
       *
       */
      fail?: (data: any, code: number) => void;

      /**
       * Called when the execution is completed.
       */
      complete?: () => void;

   }): void;

   static partitionPublic(options: {
      /**
       *
       */
      diskId: string;

      /**
       *
       */
      success?: () => void;

      /**
       *
       */
      fail?: (data: any, code: number) => void;

      /**
       * Called when the execution is completed.
       */
      complete?: () => void;

   }): void;

   static partitionPrivate(options: {
      /**
       *
       */
      diskId: string;

      /**
       *
       */
      success?: () => void;

      /**
       *
       */
      fail?: (data: any, code: number) => void;

      /**
       * Called when the execution is completed.
       */
      complete?: () => void;

   }): void;

   static getVolumes(options: {
      /**
       *
       */
      success?: (data: { volumeInfos: Array<VolumeInfo> }) => void;

      /**
       *
       */
      fail?: (data: any, code: number) => void;

      /**
       * Called when the execution is completed.
       */
      complete?: () => void;

   }): void;

   static getDisks(options: {
      /**
       *
       */
      success?: (data: { diskInfos: Array<DiskInfo> }) => void;

      /**
       *
       */
      fail?: (data: any, code: number) => void;

      /**
       * Called when the execution is completed.
       */
      complete?: () => void;

   }): void;

   static setPrimaryStorageUuid(options: {
      /**
       *
       */
      volumeUuid: string;

      /**
       *
       */
      success?: () => void;

      /**
       *
       */
      fail?: (data: any, code: number) => void;

      /**
       * Called when the execution is completed.
       */
      complete?: () => void;

   }): void;
   
   static benchMark(options: {
      /**
       *
       */
      srcUri: string;

      /**
       *
       */
      dstUri: string;

      /**
       *
       */
      success?: (uri: string) => void;

      /**
       *
       */
      fail?: (data: any, code: number) => void;

      /**
       * Called when the execution is completed.
       */
      complete?: () => void;

   }): void;

   static findVolumeById(options: {
      /**
       *
       */
      volId: string;

      /**
       *
       */
      success?: (data: { vol: Array<VolumeInfo> }) => void;

      /**
       *
       */
      fail?: (data: any, code: number) => void;

      /**
       * Called when the execution is completed.
       */
      complete?: () => void;

   }): void;

   static findVolumeByUuid(options: {
      /**
       *
       */
      volumeUuid: string;

      /**
       *
       */
      success?: (data: { vol: Array<VolumeInfo> }) => void;

      /**
       *
       */
      fail?: (data: any, code: number) => void;

      /**
       * Called when the execution is completed.
       */
      complete?: () => void;

   }): void;

   static getWritableVolumes(options: {
      /**
       *
       */
      success?: (data: { volumeInfos: Array<VolumeInfo> }) => void;

      /**
       *
       */
      fail?: (data: any, code: number) => void;

      /**
       * Called when the execution is completed.
       */
      complete?: () => void;

   }): void;

   static findDiskById(options: {
      /**
       *
       */
      dsikId: string;

      /**
       *
       */
      success?: (data: { disk: Array<DiskInfo> }) => void;

      /**
       *
       */
      fail?: (data: any, code: number) => void;

      /**
       * Called when the execution is completed.
       */
      complete?: () => void;

   }): void;

   static getPrimaryStorageUuid(options: {
      /**
       *
       */
      success?: (primaryUuid: string) => void;

      /**
       *
       */
      fail?: (data: any, code: number) => void;

      /**
       * Called when the execution is completed.
       */
      complete?: () => void;

   }): void;

   static findPrivateForEmulate(options: {
      /**
       *
       */
      emuVol: object;

      /**
       *
       */
      success?: (data: { priVol: Array<VolumeInfo> }) => void;

      /**
       *
       */
      fail?: (data: any, code: number) => void;

      /**
       * Called when the execution is completed.
       */
      complete?: () => void;

   }): void;

   static findEmulateForPrivate(options: {
      /**
       *
       */
      priVol: object;

      /**
       *
       */
      success?: (data: { emuVol: Array<VolumeInfo> }) => void;

      /**
       *
       */
      fail?: (data: any, code: number) => void;

      /**
       * Called when the execution is completed.
       */
      complete?: () => void;

   }): void;

   static getBestVolumeDescription(options: {
      /**
       *
       */
      vol: object;

      /**
       *
       */
      success?: (desCription: string) => void;

      /**
       *
       */
      fail?: (data: any, code: number) => void;

      /**
       * Called when the execution is completed.
       */
      complete?: () => void;

   }): void;
}