from  mpi4py import MPI
import numpy as np
###########
#Author: Wenkang Wei
# CPSC6770 HW 3: bucket sort
#
#Requirements: 
#     Make sure you install mpi4py and add openmpi module correctly in palmetto first and enable computing nodes to
#     use multiple cpus during testing
#
# Usage:   
#     To sort 64 elements using 8 cpus and bucket sort:
#               mpiexec -np 8 python bucketsort.py
#
#     To sort 128 elements using 8 cpus and bucket sort:
#               mpiexec -np 8 python bucketsort.py -n 128
#
#     To sort 64 elements using 4 cpus and bucket sort:
#               mpiexec -np 4 python bucketsort.py
############




def bucket_sort(comm, rank,size, status, N_num =64, array_range= (0,100), print_flag = 0):
    """
    comm: world communicator of MPI
    rank: rank of current process
    size: the number of processes in communicator
    status: world status
    N_num: amount of elements to generate in unsorted array
    array_range: range of elements in random array
    print_flag: flag indicating if print additional information or not
    
    """
    # number of process
    N_proc = size
    
    workload = int(N_num//N_proc)

    # Initialize sort, unsorted array
    sorted_array = np.zeros(N_num,dtype="int")
    unsorted_array = np.zeros(N_num,dtype="int")
    bucket_range = np.zeros(N_proc+1,dtype="int")
    # local array in each process to store partitioned sub array
    local_array = np.zeros(workload, dtype="int")
    # list of small buckets
    local_buckets = []

    if rank == 0:
        # Generate random unsorted array and range of array in process 0
        lower_bound, upper_bound = array_range
        unsorted_array = np.random.randint(low=lower_bound, high =upper_bound ,size=N_num)

        # compute range of data
        max_val = np.max(unsorted_array)
        min_val = np.min(unsorted_array)
        bucket_range = [(min_val+i*int((max_val - min_val)/N_proc)) for i in range(N_proc)]
        bucket_range.append(max_val)
        bucket_range = np.array(bucket_range, dtype= "int")

    # assign elements to local array in each process
    comm.Scatter(unsorted_array, local_array,root=0)
    # broadcast the bucket range from process 0 to other processes
    comm.Bcast(bucket_range, root = 0)
    
    if print_flag:
        # print local array, bucket range of each process
        print("Rank: ",rank, "array len: ", len(local_array),"Local array: ",local_array)
        comm.Barrier()
        print("Bucket Ranges: ", bucket_range)
        comm.Barrier()



    # list of count of elements in each small bucket
    small_bucket_count = []
    bucket_amount = N_proc
    for i in range(bucket_amount):
        # assign local array elements to small buckets
        if i == bucket_amount-1:
            small_bucket = local_array[np.logical_and(local_array>= bucket_range[i], local_array<=bucket_range[i+1])]
        else:
            small_bucket = local_array[np.logical_and(local_array>= bucket_range[i], local_array<bucket_range[i+1])]

        # Sort small bucket and get size of each small bucket 
        small_bucket.sort()
        local_buckets.append(small_bucket)
        small_bucket_count.append(len(small_bucket))

    if print_flag:
        # print to debug if print_flag is enabled
        print("Rank: ",rank,"local bucket count:", np.sum(small_bucket_count),"    " , "Local buckets: ", local_buckets)
        comm.Barrier()


    
    # Gather sizes of small buckets from different processes
    # so that we can use these sizes to construct large bucket in each process
    count_buffer = np.zeros(size* 2, dtype = "int")
    for i in range(N_proc):
        small_bucket = local_buckets[i]
        # send rank and count of elements in a small bucket to processor 
        elements_count = np.array([rank, len(small_bucket)], dtype="int")
        comm.Barrier()
        comm.Gather(elements_count, count_buffer,root = i)
        count_buffer = count_buffer.reshape([size,2])


    # Create large bucket based on gathered sizes of small buckets
    count = count_buffer[:,1]
    large_bucket = np.zeros(int(np.sum(count)) ,dtype="int")
    comm.Barrier()

    # send small buckets to large bucket
    for i in range(N_proc):
        small_bucket = local_buckets[i]
        comm.Gatherv(small_bucket, (large_bucket, count),root = i)


    # sort large bucket
    large_bucket.sort()
    if print_flag:
        # print to debug if print_flag is enabled
        print("rank: ",rank,"large bucket:", large_bucket)
        comm.Barrier()

    # gather sizes of large buckets
    large_bucket_counts = np.zeros(size, dtype="int")
    comm.Gather(np.array(len(large_bucket),dtype="int"), large_bucket_counts, root = 0)
    comm.Barrier()
    
    if rank ==0 and print_flag:
        print("Rank:", rank,"Total counts: ", np.sum(large_bucket_counts) ,"large bucket counts: ", large_bucket_counts)

    # gather large bucket into sorted array
    comm.Gatherv(large_bucket, (sorted_array, large_bucket_counts), root=0 )

    if rank ==0:
        print("Sorted array:", sorted_array)
        print()
        print("Unsorted array:", unsorted_array)
        

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description='Bucket Sort')
    # default amount in unsorted array = 64
    parser.add_argument('-n',"--num_elements", type=int, default =64,
                       help='the amount of elements in unsorted array')
    parser.add_argument('-p',"--print_flag", type=bool, default =False,
                       help='flag to enable print sorting details or not')
    args = parser.parse_args()
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    size = comm.Get_size()
    status = MPI.Status()
    print("Size: ", size, "rank:", rank)
    comm.Barrier()
    bucket_sort(comm, rank,size, status, N_num =args.num_elements, array_range= (0,200), print_flag = args.print_flag)



