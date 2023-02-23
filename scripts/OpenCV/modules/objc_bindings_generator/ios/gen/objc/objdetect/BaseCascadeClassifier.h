//
// This file is auto-generated. Please don't modify it!
//
#pragma once

#ifdef __cplusplus
//#import "opencv.hpp"
#import "opencv2/objdetect.hpp"
#import "W:/Projects/sites/library.visyond.gov/80/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/opencv\modules/objdetect\include\opencv2\objdetect.hpp"
#else
#define CV_EXPORTS
#endif

#import <Foundation/Foundation.h>
#import "Algorithm.h"





NS_ASSUME_NONNULL_BEGIN

// C++: class BaseCascadeClassifier
/**
 * The BaseCascadeClassifier module
 *
 * Member of `Objdetect`
 */
CV_EXPORTS @interface BaseCascadeClassifier : Algorithm


#ifdef __cplusplus
@property(readonly)cv::Ptr<cv::BaseCascadeClassifier> nativePtrBaseCascadeClassifier;
#endif

#ifdef __cplusplus
- (instancetype)initWithNativePtr:(cv::Ptr<cv::BaseCascadeClassifier>)nativePtr;
+ (instancetype)fromNative:(cv::Ptr<cv::BaseCascadeClassifier>)nativePtr;
#endif


#pragma mark - Methods



@end

NS_ASSUME_NONNULL_END


