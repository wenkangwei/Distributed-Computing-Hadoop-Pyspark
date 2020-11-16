from  mpi4py import MPI
import numpy as np



comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()
status = MPI.Status()
print("Size: ", size, "rank:", rank)
# number of process
N_proc = 4
N_num =64
workload = int(N_num//N_proc)

# generate 64 random numbers in array
unsorted_array = np.random.randint(low=0, high =100 ,size=N_num)
# print("Unsorted_array:", unsorted_array)
sorted_array = np.zeros(N_num)

# compute range of data
max_val = np.max(unsorted_array)
min_val = np.min(unsorted_array)
bucket_range = [min_val+i*((max_val - min_val)//N_proc) for i in range(N_proc+1)]

local_array = np.zeros(workload, dtype="int")
local_bucket = []
#local_bucket = np.zeros([N_proc,workload],dtype="int")
large_bucket = np.zeros(N_num,dtype="int")
# large_bucket = []
# assign elements to local array in each process

comm.Scatter(unsorted_array, local_array,root=0)
print(local_array)

small_bucket_count = []
# put elements into small buckets
for i in range(N_proc):
    #iterate every bucket
    count = 0
    ls = []
    for j in range(workload):
        # iterate every element
        if local_array[j]>= bucket_range[i] and local_array[j] < bucket_range[i+1]:
            #for every bucket, re-start counting index
            #local_bucket[i,count] = local_array[j]
            ls.append(local_array[j])

            count += 1
    local_bucket.append(ls)
    small_bucket_count.append(count)

# sort small buckets
#local_bucket.sort(axis=1)
print("Local bucket: ", local_bucket)

for i in range(N_proc):
    small_bucket = np.array(local_bucket[i], dtype ="int")
    small_bucket.sort()
    print("small bucket: ", small_bucket)
    comm.Gather(small_bucket, large_bucket,root = i)


#comm.Barrier()
# send small bucket elements to large bucket
#for i in range(size):    
#    tmp =local_bucket[i,len(local_bucket)-small_bucket_count[i]-1:]
#    print(tmp)
#    comm.Gather(tmp, large_bucket, root=i)

print("large bucket:", large_bucket)


# large_bucket = np.concatenate(large_bucket)
# sort large bucket
# large_bucket.sort()
# comm.Barrier()
# # send large bucket back to process 0 to get whole array
# comm.Gather(large_bucket,sorted_array,root=0)
# if rank ==0:
#     print(sorted_array)

