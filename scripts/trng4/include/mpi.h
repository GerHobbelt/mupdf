
#pragma once

#define HAS_NO_MPI

#define MPI_Init(...)
#define MPI_Comm_size(_, size_ref)                      *(size_ref) = 1
#define MPI_Comm_rank(_, rank_ref)                      *(rank_ref) = 0
#define MPI_Reduce(in_src_ptr, in_dst_ptr, ...)			*(in_dst_ptr) = *(in_src_ptr)
#define MPI_Finalize()

#define MPI_Dims_create(...)
typedef struct {
	int dummy;
} MPI_Comm;
#define MPI_Cart_create(...)
#define MPI_Cart_coords(...)

