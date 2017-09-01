export HADOOP_CLASSPATH:=${JAVA_HOME}/../lib/tools.jar
HADOOP_JARS=$(shell hadoop classpath)

all: qfdWriter.jar totalFail.jar

qfdWriter.jar: src/main/QFDWriterMapper.java src/main/QFDWriterReducer.java \
	           src/main/QFDWriterJob.java src/main/HashUtils.java \
		       src/main/WebTrafficRecord.java src/main/WTRKey.java \
               src/main/RequestReplyMatch.java src/main/QueryFocusedDataSet.java \
               src/main/QFDMatcherMapper.java src/main/QFDMatcherReducer.java
	mkdir -p classes
	javac -cp ${HADOOP_JARS}:src/main -d classes src/main/QFDWriterJob.java
	jar cfe qfdWriter.jar QFDWriterJob classes/*

totalFail.jar: src/main/TotalFailMapper.java \
	           src/main/TotalFailJob.java src/main/HashUtils.java \
		       src/main/WebTrafficRecord.java src/main/WTRKey.java \
               src/main/RequestReplyMatch.java src/main/QueryFocusedDataSet.java
	mkdir -p classes
	javac -cp ${HADOOP_JARS}:src/main -d classes src/main/TotalFailJob.java
	jar cfe totalFail.jar TotalFailJob classes/*

classes/QFDPrinter.class: src/test/QFDPrinter.java src/main/WebTrafficRecord.java \
	                      src/main/RequestReplyMatch.java src/main/QueryFocusedDataSet.java
	mkdir -p classes
	javac -cp ${HADOOP_JARS}:src/main -d classes  src/test/QFDPrinter.java

test: qfdWriter.jar totalFail.jar classes/QFDPrinter.class src/test/QFDPrinter.java
	rm -rf qfds intermediate_output
	hadoop jar qfdWriter.jar input/testvectors
	hadoop jar totalFail.jar input/tv_torip.txt
	mkdir -p output/actual
	mkdir -p qfds
	java -cp ${HADOOP_JARS}:classes QFDPrinter qfds > output/actual/test_1.txt
	diff output/expected/test_1.txt output/actual/test_1.txt

test-quiet: qfdWriter.jar totalFail.jar classes/QFDPrinter.class src/test/QFDPrinter.java
	rm -rf qfds intermediate_output
	hadoop --loglevel ERROR jar qfdWriter.jar input/testvectors
	hadoop --loglevel ERROR jar totalFail.jar input/tv_torip.txt
	mkdir -p output/actual
	mkdir -p qfds
	java -cp ${HADOOP_JARS}:classes QFDPrinter qfds > output/actual/test_1.txt
	diff output/expected/test_1.txt output/actual/test_1.txt

clean:
	rm -rf classes
	rm -f QFDPrinter.class
	rm -f *.jar
	rm -rf output/actual
