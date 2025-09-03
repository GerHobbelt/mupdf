// fake!

typedef struct {
	int bodug;
} mxArray;

typedef int mwIndex;
typedef int mwSize;

void *mxMalloc(size_t size);
void mxFree(void *ptr);

typedef enum mxArrayType {
	mxREAL = 1,

	NUM_OF_RETURN_FIELD,
} mxArrayType;

mxArray* mxCreateDoubleMatrix(size_t dim_x, size_t dim_y, mxArrayType type);
mxArray* mxCreateStructMatrix(size_t dim_x, size_t dim_y, size_t field_count, const char **field_names);
mxArray* mxDuplicateArray(mxArray* prhs);

double *mxGetPr(mxArray* ref);

int mexPrintf(const char *fmtstr, ...);

