
#define BUILD_WITH_SUITESPARSE_LIB
#define BUILD_WITH_OPENNN_MKL_LIB

#if 0
#include <Eigen/AccelerateSupport>
#endif
#include <Eigen/Cholesky>
#if defined(BUILD_WITH_SUITESPARSE_LIB)
#include <Eigen/CholmodSupport>
#endif
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Eigen>
#include <Eigen/Eigenvalues>
#include <Eigen/Geometry>
#include <Eigen/Householder>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/Jacobi>
#if defined(BUILD_WITH_SUITESPARSE_LIB)
#include <Eigen/KLUSupport>
#endif
#include <Eigen/LU>
#if defined(BUILD_WITH_SUITESPARSE_LIB)
#include <Eigen/MetisSupport>
#endif
#include <Eigen/OrderingMethods>
#if defined(BUILD_WITH_OPENNN_MKL_LIB)
#include <Eigen/PardisoSupport>
#endif
#if 0
#include <Eigen/PaStiXSupport>
#endif
#include <Eigen/QR>
#include <Eigen/QtAlignedMalloc>
#include <Eigen/Sparse>
#include <Eigen/SparseCholesky>
#include <Eigen/SparseCore>
#include <Eigen/SparseLU>
#include <Eigen/SparseQR>
#if defined(BUILD_WITH_SUITESPARSE_LIB)
#include <Eigen/SPQRSupport>
#endif



