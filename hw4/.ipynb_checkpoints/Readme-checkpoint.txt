CPSC 6770 Assignment 4
Wenkang Wei

1. Requirements:
    1.1. make sure hadoop/3.2.1-gcc/8.3.1 module and  openjdk/1.8.0_222-b10-gcc/8.3.1 have been loaded correctly
    
    1.2. make sure  /usr/bin/python3  exist so that map reduce python file can be executed by python
    
    1.3 when running run_all.sh and run_test_mapreduce.sh, they can copy movielens dataset from /zfs/citi/movielens  to local directory and copy hadoop configuration files to your home directory /home/$USER.
    

2. How to use this program:
    2.1. To run program using  mapred streaming  (recommended):
        bash run_all.sh
        
    2.2. To run program using  hdfs dfs -cat:
        bash run_test_mapreduce.sh




3. Description of files in current folder:
    run_all.sh:  the commands to start hdfs, run all map reduce programs for Question 1 and 2 in homework 4
                using mapred streaming method,  and stop hdfs
                
    run_test_mapreduce.sh:  the commands to start hdfs, run all map reduce programs for Question 1 and 2 in homework 4 using  hdfs dfs -cat method and also stop hdfs.  It is to test the mapper, reducer function 
                
    
    init_hadoop.sh:  program to initialize hadoop file system
    
    stop_hadoop.sh:  program to stop hadoop file system
    
    test_hadoop.sh: program to test hadoop file system. It is from the CPSC6770 sample codes
    
    RatingMapperReducer.ipynb:  this is the Jupyter Notebooks used to create all Mapper, Reducer Programs and all bash files





4. Description of Folders: 
  mycodes/:  this directory contains all mapper and reducer functions for homework4.
           meanRatingMapper.py， meanRatingReducer.py:   mapper, reducer pair for finding mean of ratings of each genre
                                        
           medianRatingMapper.py, medianRatingReducer.py:   mapper, reducer pair for finding median of ratings of each genre
           stdRatingMapper.py, stdRatingReducer.py:  mapper-reducer pair for finding standard deviation of ratings of each genre
           
           userRatingMapper.py, userRatingReducer.py:  mapper, reducer pair for finding user who provides most ratings and the genre this user watches most. 
           
           
  bin/:  this directory store files for initializing and shuting down hadoop file system
  
  hadoop_palmetto/:   this directory stores the configurations files of hdfs 
  
  
5. Result Output files:
    Output from run_all.sh program:
        ratings_mean.txt:  mean of ratings of each genre
        ratings_median.txt: median of ratings of each genre
        ratings_std.txt: standard deviation of ratings of each genre
        user_with_most_ratings.txt:    user who provides most ratings and the genre this user watches most
    
    Output from run_test_mapreduce.sh program:
        rating_mean_testoutput.txt:
                     mean of ratings of each genre
                     
        rating_median_testoutput.txt:
                     mean of ratings of each genre
                     
        rating_std_testoutput.txt:
                    standard deviation of ratings of each genre
                    
        user_ratings_testoutput.txt:
                    user who provides most ratings and the genre this user watches most
        