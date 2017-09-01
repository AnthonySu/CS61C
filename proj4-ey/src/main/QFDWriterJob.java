import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.MRJobConfig;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileAsBinaryOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.NullOutputFormat;
import org.apache.hadoop.mapred.lib.ChainMapper;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.NullWritable;

public class QFDWriterJob {
    private static final int NUM_MAPPERS = 8;
    private static final int NUM_REDUCERS = 8;

    public static void main(String[] args) throws Exception {

	// The first pass of the Query Focused dataset generation
	// needs to match all requests and replies which have the same
	// source IP/sport/dest IP/dest port within a 10 second window.

        Configuration conf1 = new Configuration();
        Job job1 = Job.getInstance(conf1, "QFDWriter");

        conf1.setInt(MRJobConfig.NUM_MAPS, NUM_MAPPERS);
        conf1.setInt(MRJobConfig.NUM_REDUCES, NUM_REDUCERS);

        job1.setJarByClass(QFDWriterJob.class);
        job1.setMapperClass(QFDMatcherMapper.class);
        job1.setMapOutputKeyClass(IntWritable.class);
        job1.setMapOutputValueClass(WebTrafficRecord.class);

        job1.setReducerClass(QFDMatcherReducer.class);
	job1.setOutputKeyClass(RequestReplyMatch.class);
	job1.setOutputValueClass(NullWritable.class);

        job1.setInputFormatClass(TextInputFormat.class);
        job1.setOutputFormatClass(SequenceFileOutputFormat.class);

        FileInputFormat.addInputPath(job1, new Path(args[0]));
        FileOutputFormat.setOutputPath(job1, new Path("intermediate_output"));
        job1.waitForCompletion(true);


	// Now that this is complete, we can write to the QFDs themselves...

        Configuration conf2 = new Configuration();
        Job job2 = Job.getInstance(conf2, "QFDWriter2");

        conf2.setInt(MRJobConfig.NUM_MAPS, NUM_MAPPERS);
        conf2.setInt(MRJobConfig.NUM_REDUCES, NUM_REDUCERS);

        job2.setJarByClass(QFDWriterJob.class);
        job2.setMapperClass(QFDWriterMapper.class);
        job2.setReducerClass(QFDWriterReducer.class);
        job2.setInputFormatClass(SequenceFileInputFormat.class);
	job2.setOutputFormatClass(NullOutputFormat.class);

        job2.setMapOutputKeyClass(WTRKey.class);
        job2.setMapOutputValueClass(RequestReplyMatch.class);

        FileInputFormat.addInputPath(job2, new Path("intermediate_output"));
        job2.waitForCompletion(true);
    }
}
