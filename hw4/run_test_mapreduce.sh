
module add openjdk/1.8.0_222-b10-gcc/8.3.1
module add hadoop/3.2.1-gcc/8.3.1

# copy configurations to home directory
cp -rf ./hadoop_palmetto /home/$USER/

./init_hadoop.sh

echo "Copying movielens data to current directory.."
cp -rf /zfs/citi/movielens .

# Note All mapper functions read movies.csv in ./movies.csv directory for testing
# If using hdfs dfs -cat ....| mapper.py ... to test map reduce, need to do this step 
# if using mapred streaming, we don't need to copy movies.csv to local directory 
cp ./movielens/movies.csv ./ 

echo "Data download Completed"

export HADOOP_CONF_DIR="/home/${USER}/hadoop_palmetto/config/"

# create data folder and move data to hdfs
hdfs  dfs -mkdir /user
hdfs  dfs -mkdir /user/$USER/
hdfs  dfs -mkdir /user/$USER/data
hdfs  dfs -put ./movielens data/
hdfs  dfs -ls data/movielens/

echo ""
echo "-------------------------------------"
echo "Starting MappReducer Program for HW4"
echo "-------------------------------------"

# Question 1
#Compute mean, median, std of ratings of each genre
echo "Computing Means of ratings of each genre:"
hdfs dfs -cat data/movielens/ratings.csv 2>/dev/null \
        | python mycodes/meanRatingMapper.py \
        |sort | python mycodes/meanRatingReducer.py > rating_mean_testoutput.txt

echo "Mean of ratings of each genre:"
cat rating_mean_testoutput.txt

echo "Computing Medians of ratings of each genre:"
hdfs  dfs -cat data/movielens/ratings.csv 2>/dev/null \
        | python mycodes/medianRatingMapper.py \
        |sort | python mycodes/medianRatingReducer.py > rating_median_testoutput.txt

echo "Median of ratings of each genre:"
cat rating_median_testoutput.txt

echo "Computing Standard Deviation of ratings of each genre:"
hdfs  dfs -cat data/movielens/ratings.csv 2>/dev/null \
        | python mycodes/stdRatingMapper.py \
        |sort | python mycodes/stdRatingReducer.py > rating_std_testoutput.txt

echo "Standard Deviation of ratings of each genre:"
cat rating_std_testoutput.txt

# Question 2
# Find the user who provides most ratings and the genre this user watch most
echo "Find the user who provides most ratings and the genre this user watch most:"
hdfs dfs -cat data/movielens/ratings.csv 2>/dev/null \
        | python mycodes/userRatingMapper.py \
        |sort|python mycodes/userRatingReducer.py > user_ratings_testoutput.txt

cat user_ratings_testoutput.txt
./stop_hadoop.sh
