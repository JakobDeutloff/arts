template
<
typename LocalType,
template <typename> typename StorageTemplate
>
MPIMatrix<LocalType, StorageTemplate>::MPIMatrix()
    : local(), local_rows(0)
{
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    unsigned int index = 0;
    row_indices.reserve(nprocs);
    row_ranges.reserve(nprocs);

    for (unsigned int i = 0; i < nprocs; i++)
    {
        row_indices.push_back(0);
        row_ranges.push_back(0);
    }

    m = 0;
    n = 0;
}

// template
// <
// typename LocalType,
// template <typename> typename StorageTemplate
// >
// auto MPIMatrix<LocalType, StorageTemplate>::operator=(const MPIMatrix &A)
//     -> MPIMatrix &
// {
//     local = A.local;
//     local_rows = A.local_rows;

//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//     MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

//     int *proc_rows = new int[nprocs];
//     broadcast_local_rows(proc_rows);

//     unsigned int index = 0;
//     row_indices.reserve(nprocs);
//     row_ranges.reserve(nprocs);

//     for (unsigned int i = 0; i < nprocs; i++)
//     {
//         row_indices.push_back(index);
//         row_ranges.push_back(proc_rows[i]);
//         index += proc_rows[i];
//     }

//     m = index;
//     n = local.cols();
// }

template
<
typename LocalType,
template <typename> typename StorageTemplate
>
template<typename T, typename, typename>
MPIMatrix<LocalType, StorageTemplate>::MPIMatrix(T &&local_matrix)
    : local(std::forward<T>(local_matrix)),
      local_rows(remove_reference_wrapper(local).rows())
{
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int *proc_rows = new int[nprocs];
    broadcast_local_rows(proc_rows);

    unsigned int index = 0;
    row_indices.reserve(nprocs);
    row_ranges.reserve(nprocs);

    for (unsigned int i = 0; i < nprocs; i++)
    {
        row_indices.push_back(index);
        row_ranges.push_back(proc_rows[i]);
        index += proc_rows[i];
    }

    m = index;
    n = remove_reference_wrapper(local).cols();
}

template
<
typename LocalType,
template <typename> typename StorageTemplate
>
MPIMatrix<LocalType, StorageTemplate>::MPIMatrix(const LocalType &local_matrix)
    : local(local_matrix), local_rows(remove_reference_wrapper(local).rows())
{
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int *proc_rows = new int[nprocs];
    broadcast_local_rows(proc_rows);

    unsigned int index = 0;
    row_indices.reserve(nprocs);
    row_ranges.reserve(nprocs);

    for (unsigned int i = 0; i < nprocs; i++)
    {
        row_indices.push_back(index);
        row_ranges.push_back(proc_rows[i]);
        index += proc_rows[i];
    }

    m = index;
    n = remove_reference_wrapper(local).cols();
}

template
<
typename LocalType,
template <typename> typename StorageTemplate
>
auto MPIMatrix<LocalType, StorageTemplate>::split_matrix(const MatrixType &local_matrix)
    -> MPIMatrix<LocalType, LValue>
{
    int rank;
    int nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    // Distribute rows evenly over MPI processes.
    unsigned int total_rows = local_matrix.rows();
    unsigned int local_rows = total_rows / nprocs;
    unsigned int remainder = total_rows % nprocs;
    unsigned int local_start = local_rows * rank;


    if (rank < remainder)
    {
        local_rows += 1;
        local_start += rank;
    }
    else
    {
        local_start += remainder;
    }

    unsigned int n = local_matrix.cols();
    LocalType block = local_matrix.get_block(local_start, 0, local_rows, n);
    MPIMatrix<LocalType, LValue> splitted_matrix(block);
    return splitted_matrix;
}

template
<
typename LocalType,
template <typename> typename StorageTemplate
>
auto MPIMatrix<LocalType, StorageTemplate>::resize(unsigned int i,
                                                   unsigned int j)
      -> void
{
    m = i; n = j;

    // Distribute rows evenly over MPI processes.
    unsigned int total_rows = m;
    local_rows = total_rows / nprocs;
    unsigned int remainder = total_rows % nprocs;
    unsigned int local_start = local_rows * rank;

    if (rank < remainder)
    {
        local_rows += 1;
        local_start += rank;
    }
    else
    {
        local_start += remainder;
    }

    int *proc_rows = new int[nprocs];
    broadcast_local_rows(proc_rows);

    unsigned int index = 0;
    row_indices.reserve(nprocs);
    row_ranges.reserve(nprocs);

    for (unsigned int i = 0; i < nprocs; i++)
    {
        row_indices[i] = index;
        row_ranges[i]  = proc_rows[i];
        index += proc_rows[i];
    }

    local.resize(local_rows, j);
}

template
<
typename LocalType,
template <typename> typename StorageTemplate
>
auto MPIMatrix<LocalType, StorageTemplate>::broadcast(LocalType &local)
    -> void
{
    int m = local.rows();
    int n = local.cols();

    MPI_Bcast(&m, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);

    MPI_Bcast(local.raw_pointer(), m * n, mpi_data_type, 0, MPI_COMM_WORLD);
}

template
<
typename LocalType,
template <typename> typename StorageTemplate
>
auto MPIMatrix<LocalType, StorageTemplate>::rows() const
    -> unsigned int
{
    return m;
}

template
<
typename LocalType,
template <typename> typename StorageTemplate
>
auto MPIMatrix<LocalType, StorageTemplate>::cols() const
    -> unsigned int
{
    return n;
}

template
<
typename LocalType,
template <typename> typename StorageTemplate
>
auto MPIMatrix<LocalType, StorageTemplate>::get_local()
    -> LocalType &
{
    return local;
}


template
<
typename LocalType,
template <typename> typename StorageTemplate
>
auto MPIMatrix<LocalType, StorageTemplate>::operator()(unsigned int i,
                                                       unsigned int j) const
    -> RealType
{
    int owner;
    for (int r = 0; r < nprocs; r++)
    {
        if ((i >= row_indices[r]) && (i < row_indices[r] + row_ranges[r]))
            owner = r;
    }

    if (rank == owner)
        local_element = local(i - row_indices[rank], j);

    MPI_Bcast(&local_element, 1, mpi_data_type, owner, MPI_COMM_WORLD);

    if (rank == owner)
        return local(i - row_indices[rank], j);
    else
        return local_element;
}

template
<
typename LocalType,
template <typename> typename StorageTemplate
>
auto MPIMatrix<LocalType, StorageTemplate>::operator()(unsigned int i,
                                                       unsigned int j)
    -> RealType &
{
    int owner = 0;
    for (int r = 0; r < nprocs; r++)
    {
        if ((i >= row_indices[r]) && (i < row_indices[r] + row_ranges[r]))
            owner = r;
    }

    if (rank == owner)
        local_element = local(i - row_indices[rank], j);

    MPI_Bcast(&local_element, 1, mpi_data_type, owner, MPI_COMM_WORLD);

    if (rank == owner)
        return local(i - row_indices[rank], j);
    else
        return local_element;
}

template
<
typename LocalType,
template <typename> typename StorageTemplate
>
auto MPIMatrix<LocalType, StorageTemplate>::multiply(const VectorType &v) const
    -> VectorType
{
    VectorType w{}; w.resize(m);
    VectorType w_local{}; w_local.resize(local_rows);
    w_local = remove_reference_wrapper(local).multiply(v);
    broadcast_local_block(w.raw_pointer(), w_local.raw_pointer());
    return w;
}

template
<
typename LocalType,
template <typename> typename StorageTemplate
>
auto MPIMatrix<LocalType, StorageTemplate>::transpose_multiply(const VectorType &v) const
    -> VectorType
{
    VectorType w_local{};   w_local.resize(n);
    VectorType w{};   w.resize(n);

    w_local =
        remove_reference_wrapper(local).transpose_multiply_block(v, row_indices[rank], row_ranges[rank]);
    reduce_vector_sum(w.raw_pointer(), w_local.raw_pointer());
    return w;
}

// template
// <
// typename LocalType,
// template <typename> typename StorageTemplate
// >
// MPIMatrix<LocalType, StorageTemplate>::operator MPIMatrix<LocalType, ConstRef>() const
// {
//     return MPIMatrix<LocalType, ConstRef>(local);
// }

// template
// <
// typename LocalType,
// template <typename> typename StorageTemplate
// >
// MPIMatrix<LocalType, StorageTemplate>::operator LocalType()
// {
//     LocalType A; A.resize(m, n);
//     auto matrix_buffer = A.raw_pointer();
//     auto start  = matrix_buffer + row_indices[rank] * n;
//     auto length = row_ranges[rank] * n;

//     std::copy(local.raw_pointer(), local.raw_pointer() + length, start);

//     for (int i = 0; i < nprocs; i++)
//     {
//         start  = matrix_buffer + row_indices[i] * n;
//         length = row_ranges[i] * n;
//         MPI_Bcast(start, length, mpi_data_type, i, MPI_COMM_WORLD);
//     }
//     return A;
// }

template
<
typename LocalType,
template <typename> typename StorageTemplate
>
auto MPIMatrix<LocalType, StorageTemplate>::broadcast_local_rows(int *rows) const
    -> void
{
    int sendbuf = local_rows;
    MPI_Allgather(&sendbuf, 1, MPI_INTEGER, rows, 1, MPI_INTEGER, MPI_COMM_WORLD);
}

template
<
typename LocalType,
template <typename> typename StorageTemplate
>
auto MPIMatrix<LocalType, StorageTemplate>::broadcast_local_block(double *vector,
                                                                  const double *block) const
    -> void
{
    memcpy(vector + row_indices[rank], block, row_ranges[rank] * sizeof(double));
    for (unsigned int i = 0; i < nprocs; i++)
    {
        MPI_Bcast(vector + row_indices[i], row_ranges[i], mpi_data_type,
                  i, MPI_COMM_WORLD);
    }
}

template
<
typename LocalType,
template <typename> typename StorageTemplate
>
auto MPIMatrix<LocalType, StorageTemplate>::reduce_vector_sum(double *result_vector,
                                             double *local_vector) const
    -> void
{
    memset(result_vector, 0, n * sizeof(double));
    MPI_Allreduce(local_vector, result_vector, n, mpi_data_type,
                  MPI_SUM, MPI_COMM_WORLD);
}