import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.MRJobConfig;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.NullOutputFormat;

public class TotalFailJob {
    public static final int NUM_MAPPERS = 8;
    public static final int NUM_REDUCERS = 8;


    public static void main(String[] args) throws Exception {
        Configuration conf = new Configuration();
        conf.setInt(MRJobConfig.NUM_MAPS, NUM_MAPPERS);
        conf.setInt(MRJobConfig.NUM_REDUCES, NUM_REDUCERS);
        Job job = Job.getInstance(conf, "TotalFail");
        job.setJarByClass(TotalFailJob.class);

        job.setMapperClass(TotalFailMapper.class);
        job.setMapOutputKeyClass(WTRKey.class);
        job.setMapOutputValueClass(RequestReplyMatch.class);
        job.setReducerClass(QFDWriterReducer.class);

        job.setInputFormatClass(TextInputFormat.class);
        job.setOutputFormatClass(NullOutputFormat.class);

        FileInputFormat.addInputPath(job, new Path(args[0]));
        job.waitForCompletion(true);
    }
}
