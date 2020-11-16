
module add openjdk/1.8.0_222-b10-gcc/8.3.1
module add hadoop/3.2.1-gcc/8.3.1

echo "Hadoop config file to home directory"
cp -rf ./hadoop_palmetto /home/$USER/

sleep 2

./init_hadoop.sh


echo "Copying movielens data to current directory and HDFS ..."
cp -rf /zfs/citi/movielens .


export HADOOP_CONF_DIR="/home/${USER}/hadoop_palmetto/config/"

# # create data folder and move data to hdfs
hdfs  dfs -mkdir /user
hdfs  dfs -mkdir /user/$USER/
hdfs  dfs -mkdir /user/$USER/data
hdfs  dfs -put ./movielens data/
hdfs  dfs -ls data/movielens/

echo "Data download Completed"

sleep 3


hdfs dfs -rm -r data/ratings_mean/
hdfs dfs -rm -r data/ratings_median/
hdfs dfs -rm -r data/ratings_std/
hdfs dfs -rm -r data/user_with_most_ratings/


echo ""
echo "-------------------------------------"
echo "Starting MappReducer Program for HW4"
echo "-------------------------------------"

echo ""
echo "Finding Mean of ratings of each genre:"
echo ""

sleep 2

mapred streaming \
    -input data/movielens/ratings.csv \
    -output data/ratings_mean \
    -file ./mycodes/meanRatingMapper.py \
    -mapper meanRatingMapper.py \
    -file ./mycodes/meanRatingReducer.py \
    -reducer meanRatingReducer.py \
    -file ./movielens/movies.csv

hdfs dfs -ls data/ratings_mean/
echo ""
echo " Mean of ratings of each genre:"
echo ""
hdfs dfs -cat data/ratings_mean/part-00000

echo "Copy result to local directory"
hdfs dfs -get data/ratings_mean/part-00000 ./ratings_mean.txt


echo ""
echo "Finding Median of ratings of each genre.."
echo ""

sleep 2

mapred streaming \
    -input data/movielens/ratings.csv \
    -output data/ratings_median \
    -file ./mycodes/medianRatingMapper.py \
    -mapper medianRatingMapper.py \
    -file ./mycodes/medianRatingReducer.py \
    -reducer medianRatingReducer.py \
    -file ./movielens/movies.csv


hdfs dfs -ls data/ratings_median/

echo ""
echo " Median of ratings of each genre:"
echo ""
hdfs dfs -cat data/ratings_median/part-00000

echo "Copy result to local directory"
hdfs dfs -get data/ratings_median/part-00000 ./ratings_median.txt



echo ""
echo "Finding Standard deviation of ratings of each genre.."
echo ""

sleep 2

mapred streaming \
    -input data/movielens/ratings.csv \
    -output data/ratings_std \
    -file ./mycodes/stdRatingMapper.py \
    -mapper stdRatingMapper.py \
    -file ./mycodes/stdRatingReducer.py \
    -reducer stdRatingReducer.py \
    -file ./movielens/movies.csv


hdfs dfs -ls data/ratings_std/

echo ""
echo " Standard deviation of ratings of each genre:"
echo ""
hdfs dfs -cat data/ratings_std/part-00000

echo "Copy result to local directory"
hdfs dfs -get data/ratings_std/part-00000 ./ratings_std.txt


echo ""
echo "Finding  User who provides most rating and the genre this user watches most:"
echo ""

sleep 2

mapred streaming \
    -input data/movielens/ratings.csv \
    -output data/user_with_most_ratings \
    -file ./mycodes/userRatingMapper.py \
    -mapper userRatingMapper.py \
    -file ./mycodes/userRatingReducer.py \
    -reducer userRatingReducer.py \
    -file ./movielens/movies.csv

hdfs dfs -ls data/user_with_most_ratings/

echo ""
echo " User who provides most rating and the genre this user watches most:"
echo ""
hdfs dfs -cat data/user_with_most_ratings/part-00000

echo "Copy result to local directory"
hdfs dfs -get data/user_with_most_ratings/part-00000 ./user_with_most_ratings.txt

echo "Program Completed"
./stop_hadoop.sh
