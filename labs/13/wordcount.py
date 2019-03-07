import sys
import re

from pyspark import SparkContext

def flat_map(document):
    """Returns back all of the words in LINE """

    # This is a regex library function which returns a list
    # of all the instances of the specified regex in a string.

    # In our case, the regex is the r"\w+" thing, and the
    # string to search in is document[1], which is the text
    # of the document. (document[0], on the other hand, is
    # the ID number of the document).
    return re.findall(r"\w+", document[1])

def map(word):
    """ Create pairs where the word is the key and 1 is the value """
    return (word, 1)

def reduce(a, b):
    """ Add up the counts for a word """
    return a + b

def wordcount(file_name, output="spark-wc-out-wordcount"):
    """ Reads in a sequence file FILE_NAME to be manipulated """
    
    # We'll be using this to call Spark built-in functions!
    sc = SparkContext("local[8]", "WordCount")
    
    # This gets us an RDD object, on which we can apply the Spark
    # built-in functions! In particular, file is an RDD object
    # which contains all the documents specified by the sequence file
    # passed into this function. 

    # What do I mean by "contains all the documents"? If you
    # import this file from the interpreter (pyspark), and you ran
    # this line of code, in addition to file.take(n), you would
    # get the text of the first n documents specified by the sequence file input.
    file = sc.sequenceFile(file_name)

    """
    - flatMap takes in a function that will take one input and outputs 0 or more
      items. IMPORTANT: It then applies that function to every one of its elements,
      and condenses them into a single RDD. In our case, that means it will apply
      our flat_map function to every DOCUMENT in file, and then combine the results
      into a SINGLE RDD.
    - map takes in a function that will take one input and outputs a single item
    - reduceByKey takes in a function, groups the dataset by keys and aggregates
      the values of each key
    """
    counts = file.flatMap(flat_map) \
                 .map(map) \
                 .reduceByKey(reduce)

    """ Takes the dataset stored in counts and writes everything out to OUTPUT """
    counts.coalesce(1).saveAsTextFile(output)

""" Do not worry about this """
if __name__ == "__main__":
    argv = sys.argv
    if len(argv) == 2:
        wordcount(argv[1])
    else:
        wordcount(argv[1], argv[2])
