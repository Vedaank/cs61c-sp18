import sys
import re

from pyspark import SparkContext

def flat_map(document):
    """
    In the current framework, this function
    takes in a document, which is a key, value pair, where document[0] is the
    document ID and document[1] is the contents of the document. 

    One idea is to pass a distinct list of all the words in each document to our map
    function. There is an RDD transformation function called ".distinct()" which throws
    away redundant copies, but if we call use it on an RDD with ALL the words from
    ALL the documents (which is what we currently have after this function returns),
    we won't get quite what we want (why?). 

    HINT: This function doesn't have to return a single word... it could return a tuple!
    """

    """ Replace or modify this line! """
    return re.findall(r"\w+", document[1])

def map(arg):
    """ Replace or modify this line. """
    return (arg, arg)

def reduce(arg1, arg2):
    """ Replace or modify this line. """
    return arg1

def docwordcount(file_name, output="spark-wc-out-docwordcount"):
    # These two lines of code are identical from wordcount.py
    sc = SparkContext("local[8]", "DocWordCount")
    file = sc.sequenceFile(file_name)

    # This is the given framework for the function. We urge
    # you not to change it TOO much.
    # You CAN complete the exercise by MOSTLY modifying the
    # functions above, but you ARE free to change this.
    # In particular, you'll probably want to add a
    # transformation at the very end to sort stuff...
    counts = file.flatMap(flat_map) \
                 .map(map) \
                 .reduceByKey(reduce)

    counts.coalesce(1).saveAsTextFile(output)

""" Do not worry about this """
if __name__ == "__main__":
    argv = sys.argv
    if len(argv) == 2:
        docwordcount(argv[1])
    else:
        docwordcount(argv[1], argv[2])
