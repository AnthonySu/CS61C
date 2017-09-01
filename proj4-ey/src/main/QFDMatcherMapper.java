import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.io.IntWritable;

import java.io.IOException;

public class QFDMatcherMapper extends Mapper<LongWritable, Text,
				      IntWritable, WebTrafficRecord> {

    @Override
    public void map(LongWritable lineNo, Text line, Context ctxt)
	throws IOException, InterruptedException {
        // Inputs come on lines of text that can be parsed
        // as WebTrafficRecord, your key should be such that all
        // records with the same source IP/source port/dest IP/dest port
        // are the same so they always go to the same reducer...


        // Using the parseFromLine
        WebTrafficRecord new_record = WebTrafficRecord.parseFromLine(line.toString());
        // Therefore, we could get the hashCode from new_record
        IntWritable hashCode = new IntWritable(new_record.matchHashCode());
        // Write to the context
        ctxt.write(hashCode, new_record);
    }
}
