//
// This file is auto-generated. Please don't modify it!
//
#pragma once

#ifdef __cplusplus
//#import "opencv.hpp"
#import "opencv2/features2d.hpp"
#import "../../../../thirdparty/owemdjee/opencv\modules/features2d\include\opencv2\features2d.hpp"
#else
#define CV_EXPORTS
#endif

#import <Foundation/Foundation.h>
#import "Feature2D.h"





NS_ASSUME_NONNULL_BEGIN

// C++: class SIFT
/**
 * Class for extracting keypoints and computing descriptors using the Scale Invariant Feature Transform
 * (SIFT) algorithm by D. Lowe CITE: Lowe04 .
 *
 * Member of `Features2d`
 */
CV_EXPORTS @interface SIFT : Feature2D


#ifdef __cplusplus
@property(readonly)cv::Ptr<cv::SIFT> nativePtrSIFT;
#endif

#ifdef __cplusplus
- (instancetype)initWithNativePtr:(cv::Ptr<cv::SIFT>)nativePtr;
+ (instancetype)fromNative:(cv::Ptr<cv::SIFT>)nativePtr;
#endif


#pragma mark - Methods


//
// static Ptr_SIFT cv::SIFT::create(int nfeatures = 0, int nOctaveLayers = 3, double contrastThreshold = 0.04, double edgeThreshold = 10, double sigma = 1.6, bool enable_precise_upscale = false)
//
/**
 * @param nfeatures The number of best features to retain. The features are ranked by their scores
 *     (measured in SIFT algorithm as the local contrast)
 *
 * @param nOctaveLayers The number of layers in each octave. 3 is the value used in D. Lowe paper. The
 *     number of octaves is computed automatically from the image resolution.
 *
 * @param contrastThreshold The contrast threshold used to filter out weak features in semi-uniform
 *     (low-contrast) regions. The larger the threshold, the less features are produced by the detector.
 *
 *     NOTE: The contrast threshold will be divided by nOctaveLayers when the filtering is applied. When
 *     nOctaveLayers is set to default and if you want to use the value used in D. Lowe paper, 0.03, set
 *     this argument to 0.09.
 *
 * @param edgeThreshold The threshold used to filter out edge-like features. Note that the its meaning
 *     is different from the contrastThreshold, i.e. the larger the edgeThreshold, the less features are
 *     filtered out (more features are retained).
 *
 * @param sigma The sigma of the Gaussian applied to the input image at the octave \#0. If your image
 *     is captured with a weak camera with soft lenses, you might want to reduce the number.
 *
 * @param enable_precise_upscale Whether to enable precise upscaling in the scale pyramid, which maps
 *     index `$$\texttt{x}$$` to `$$\texttt{2x}$$`. This prevents localization bias. The option
 *     is disabled by default.
 */
+ (SIFT*)create:(int)nfeatures nOctaveLayers:(int)nOctaveLayers contrastThreshold:(double)contrastThreshold edgeThreshold:(double)edgeThreshold sigma:(double)sigma enable_precise_upscale:(BOOL)enable_precise_upscale NS_SWIFT_NAME(create(nfeatures:nOctaveLayers:contrastThreshold:edgeThreshold:sigma:enable_precise_upscale:));

/**
 * @param nfeatures The number of best features to retain. The features are ranked by their scores
 *     (measured in SIFT algorithm as the local contrast)
 *
 * @param nOctaveLayers The number of layers in each octave. 3 is the value used in D. Lowe paper. The
 *     number of octaves is computed automatically from the image resolution.
 *
 * @param contrastThreshold The contrast threshold used to filter out weak features in semi-uniform
 *     (low-contrast) regions. The larger the threshold, the less features are produced by the detector.
 *
 *     NOTE: The contrast threshold will be divided by nOctaveLayers when the filtering is applied. When
 *     nOctaveLayers is set to default and if you want to use the value used in D. Lowe paper, 0.03, set
 *     this argument to 0.09.
 *
 * @param edgeThreshold The threshold used to filter out edge-like features. Note that the its meaning
 *     is different from the contrastThreshold, i.e. the larger the edgeThreshold, the less features are
 *     filtered out (more features are retained).
 *
 * @param sigma The sigma of the Gaussian applied to the input image at the octave \#0. If your image
 *     is captured with a weak camera with soft lenses, you might want to reduce the number.
 *
 *     index `$$\texttt{x}$$` to `$$\texttt{2x}$$`. This prevents localization bias. The option
 *     is disabled by default.
 */
+ (SIFT*)create:(int)nfeatures nOctaveLayers:(int)nOctaveLayers contrastThreshold:(double)contrastThreshold edgeThreshold:(double)edgeThreshold sigma:(double)sigma NS_SWIFT_NAME(create(nfeatures:nOctaveLayers:contrastThreshold:edgeThreshold:sigma:));

/**
 * @param nfeatures The number of best features to retain. The features are ranked by their scores
 *     (measured in SIFT algorithm as the local contrast)
 *
 * @param nOctaveLayers The number of layers in each octave. 3 is the value used in D. Lowe paper. The
 *     number of octaves is computed automatically from the image resolution.
 *
 * @param contrastThreshold The contrast threshold used to filter out weak features in semi-uniform
 *     (low-contrast) regions. The larger the threshold, the less features are produced by the detector.
 *
 *     NOTE: The contrast threshold will be divided by nOctaveLayers when the filtering is applied. When
 *     nOctaveLayers is set to default and if you want to use the value used in D. Lowe paper, 0.03, set
 *     this argument to 0.09.
 *
 * @param edgeThreshold The threshold used to filter out edge-like features. Note that the its meaning
 *     is different from the contrastThreshold, i.e. the larger the edgeThreshold, the less features are
 *     filtered out (more features are retained).
 *
 *     is captured with a weak camera with soft lenses, you might want to reduce the number.
 *
 *     index `$$\texttt{x}$$` to `$$\texttt{2x}$$`. This prevents localization bias. The option
 *     is disabled by default.
 */
+ (SIFT*)create:(int)nfeatures nOctaveLayers:(int)nOctaveLayers contrastThreshold:(double)contrastThreshold edgeThreshold:(double)edgeThreshold NS_SWIFT_NAME(create(nfeatures:nOctaveLayers:contrastThreshold:edgeThreshold:));

/**
 * @param nfeatures The number of best features to retain. The features are ranked by their scores
 *     (measured in SIFT algorithm as the local contrast)
 *
 * @param nOctaveLayers The number of layers in each octave. 3 is the value used in D. Lowe paper. The
 *     number of octaves is computed automatically from the image resolution.
 *
 * @param contrastThreshold The contrast threshold used to filter out weak features in semi-uniform
 *     (low-contrast) regions. The larger the threshold, the less features are produced by the detector.
 *
 *     NOTE: The contrast threshold will be divided by nOctaveLayers when the filtering is applied. When
 *     nOctaveLayers is set to default and if you want to use the value used in D. Lowe paper, 0.03, set
 *     this argument to 0.09.
 *
 *     is different from the contrastThreshold, i.e. the larger the edgeThreshold, the less features are
 *     filtered out (more features are retained).
 *
 *     is captured with a weak camera with soft lenses, you might want to reduce the number.
 *
 *     index `$$\texttt{x}$$` to `$$\texttt{2x}$$`. This prevents localization bias. The option
 *     is disabled by default.
 */
+ (SIFT*)create:(int)nfeatures nOctaveLayers:(int)nOctaveLayers contrastThreshold:(double)contrastThreshold NS_SWIFT_NAME(create(nfeatures:nOctaveLayers:contrastThreshold:));

/**
 * @param nfeatures The number of best features to retain. The features are ranked by their scores
 *     (measured in SIFT algorithm as the local contrast)
 *
 * @param nOctaveLayers The number of layers in each octave. 3 is the value used in D. Lowe paper. The
 *     number of octaves is computed automatically from the image resolution.
 *
 *     (low-contrast) regions. The larger the threshold, the less features are produced by the detector.
 *
 *     NOTE: The contrast threshold will be divided by nOctaveLayers when the filtering is applied. When
 *     nOctaveLayers is set to default and if you want to use the value used in D. Lowe paper, 0.03, set
 *     this argument to 0.09.
 *
 *     is different from the contrastThreshold, i.e. the larger the edgeThreshold, the less features are
 *     filtered out (more features are retained).
 *
 *     is captured with a weak camera with soft lenses, you might want to reduce the number.
 *
 *     index `$$\texttt{x}$$` to `$$\texttt{2x}$$`. This prevents localization bias. The option
 *     is disabled by default.
 */
+ (SIFT*)create:(int)nfeatures nOctaveLayers:(int)nOctaveLayers NS_SWIFT_NAME(create(nfeatures:nOctaveLayers:));

/**
 * @param nfeatures The number of best features to retain. The features are ranked by their scores
 *     (measured in SIFT algorithm as the local contrast)
 *
 *     number of octaves is computed automatically from the image resolution.
 *
 *     (low-contrast) regions. The larger the threshold, the less features are produced by the detector.
 *
 *     NOTE: The contrast threshold will be divided by nOctaveLayers when the filtering is applied. When
 *     nOctaveLayers is set to default and if you want to use the value used in D. Lowe paper, 0.03, set
 *     this argument to 0.09.
 *
 *     is different from the contrastThreshold, i.e. the larger the edgeThreshold, the less features are
 *     filtered out (more features are retained).
 *
 *     is captured with a weak camera with soft lenses, you might want to reduce the number.
 *
 *     index `$$\texttt{x}$$` to `$$\texttt{2x}$$`. This prevents localization bias. The option
 *     is disabled by default.
 */
+ (SIFT*)create:(int)nfeatures NS_SWIFT_NAME(create(nfeatures:));

/**
 *     (measured in SIFT algorithm as the local contrast)
 *
 *     number of octaves is computed automatically from the image resolution.
 *
 *     (low-contrast) regions. The larger the threshold, the less features are produced by the detector.
 *
 *     NOTE: The contrast threshold will be divided by nOctaveLayers when the filtering is applied. When
 *     nOctaveLayers is set to default and if you want to use the value used in D. Lowe paper, 0.03, set
 *     this argument to 0.09.
 *
 *     is different from the contrastThreshold, i.e. the larger the edgeThreshold, the less features are
 *     filtered out (more features are retained).
 *
 *     is captured with a weak camera with soft lenses, you might want to reduce the number.
 *
 *     index `$$\texttt{x}$$` to `$$\texttt{2x}$$`. This prevents localization bias. The option
 *     is disabled by default.
 */
+ (SIFT*)create NS_SWIFT_NAME(create());


//
// static Ptr_SIFT cv::SIFT::create(int nfeatures, int nOctaveLayers, double contrastThreshold, double edgeThreshold, double sigma, int descriptorType, bool enable_precise_upscale = false)
//
/**
 * Create SIFT with specified descriptorType.
 * @param nfeatures The number of best features to retain. The features are ranked by their scores
 *     (measured in SIFT algorithm as the local contrast)
 *
 * @param nOctaveLayers The number of layers in each octave. 3 is the value used in D. Lowe paper. The
 *     number of octaves is computed automatically from the image resolution.
 *
 * @param contrastThreshold The contrast threshold used to filter out weak features in semi-uniform
 *     (low-contrast) regions. The larger the threshold, the less features are produced by the detector.
 *
 *     NOTE: The contrast threshold will be divided by nOctaveLayers when the filtering is applied. When
 *     nOctaveLayers is set to default and if you want to use the value used in D. Lowe paper, 0.03, set
 *     this argument to 0.09.
 *
 * @param edgeThreshold The threshold used to filter out edge-like features. Note that the its meaning
 *     is different from the contrastThreshold, i.e. the larger the edgeThreshold, the less features are
 *     filtered out (more features are retained).
 *
 * @param sigma The sigma of the Gaussian applied to the input image at the octave \#0. If your image
 *     is captured with a weak camera with soft lenses, you might want to reduce the number.
 *
 * @param descriptorType The type of descriptors. Only CV_32F and CV_8U are supported.
 *
 * @param enable_precise_upscale Whether to enable precise upscaling in the scale pyramid, which maps
 *     index `$$\texttt{x}$$` to `$$\texttt{2x}$$`. This prevents localization bias. The option
 *     is disabled by default.
 */
+ (SIFT*)create:(int)nfeatures nOctaveLayers:(int)nOctaveLayers contrastThreshold:(double)contrastThreshold edgeThreshold:(double)edgeThreshold sigma:(double)sigma descriptorType:(int)descriptorType enable_precise_upscale:(BOOL)enable_precise_upscale NS_SWIFT_NAME(create(nfeatures:nOctaveLayers:contrastThreshold:edgeThreshold:sigma:descriptorType:enable_precise_upscale:));

/**
 * Create SIFT with specified descriptorType.
 * @param nfeatures The number of best features to retain. The features are ranked by their scores
 *     (measured in SIFT algorithm as the local contrast)
 *
 * @param nOctaveLayers The number of layers in each octave. 3 is the value used in D. Lowe paper. The
 *     number of octaves is computed automatically from the image resolution.
 *
 * @param contrastThreshold The contrast threshold used to filter out weak features in semi-uniform
 *     (low-contrast) regions. The larger the threshold, the less features are produced by the detector.
 *
 *     NOTE: The contrast threshold will be divided by nOctaveLayers when the filtering is applied. When
 *     nOctaveLayers is set to default and if you want to use the value used in D. Lowe paper, 0.03, set
 *     this argument to 0.09.
 *
 * @param edgeThreshold The threshold used to filter out edge-like features. Note that the its meaning
 *     is different from the contrastThreshold, i.e. the larger the edgeThreshold, the less features are
 *     filtered out (more features are retained).
 *
 * @param sigma The sigma of the Gaussian applied to the input image at the octave \#0. If your image
 *     is captured with a weak camera with soft lenses, you might want to reduce the number.
 *
 * @param descriptorType The type of descriptors. Only CV_32F and CV_8U are supported.
 *
 *     index `$$\texttt{x}$$` to `$$\texttt{2x}$$`. This prevents localization bias. The option
 *     is disabled by default.
 */
+ (SIFT*)create:(int)nfeatures nOctaveLayers:(int)nOctaveLayers contrastThreshold:(double)contrastThreshold edgeThreshold:(double)edgeThreshold sigma:(double)sigma descriptorType:(int)descriptorType NS_SWIFT_NAME(create(nfeatures:nOctaveLayers:contrastThreshold:edgeThreshold:sigma:descriptorType:));


//
//  String cv::SIFT::getDefaultName()
//
- (NSString*)getDefaultName NS_SWIFT_NAME(getDefaultName());


//
//  void cv::SIFT::setNFeatures(int maxFeatures)
//
- (void)setNFeatures:(int)maxFeatures NS_SWIFT_NAME(setNFeatures(maxFeatures:));


//
//  int cv::SIFT::getNFeatures()
//
- (int)getNFeatures NS_SWIFT_NAME(getNFeatures());


//
//  void cv::SIFT::setNOctaveLayers(int nOctaveLayers)
//
- (void)setNOctaveLayers:(int)nOctaveLayers NS_SWIFT_NAME(setNOctaveLayers(nOctaveLayers:));


//
//  int cv::SIFT::getNOctaveLayers()
//
- (int)getNOctaveLayers NS_SWIFT_NAME(getNOctaveLayers());


//
//  void cv::SIFT::setContrastThreshold(double contrastThreshold)
//
- (void)setContrastThreshold:(double)contrastThreshold NS_SWIFT_NAME(setContrastThreshold(contrastThreshold:));


//
//  double cv::SIFT::getContrastThreshold()
//
- (double)getContrastThreshold NS_SWIFT_NAME(getContrastThreshold());


//
//  void cv::SIFT::setEdgeThreshold(double edgeThreshold)
//
- (void)setEdgeThreshold:(double)edgeThreshold NS_SWIFT_NAME(setEdgeThreshold(edgeThreshold:));


//
//  double cv::SIFT::getEdgeThreshold()
//
- (double)getEdgeThreshold NS_SWIFT_NAME(getEdgeThreshold());


//
//  void cv::SIFT::setSigma(double sigma)
//
- (void)setSigma:(double)sigma NS_SWIFT_NAME(setSigma(sigma:));


//
//  double cv::SIFT::getSigma()
//
- (double)getSigma NS_SWIFT_NAME(getSigma());



@end

NS_ASSUME_NONNULL_END


