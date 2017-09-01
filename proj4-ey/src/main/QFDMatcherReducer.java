import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import java.io.IOException;
import java.lang.Math;
import java.util.ArrayList;
public class QFDMatcherReducer extends Reducer<IntWritable, WebTrafficRecord, RequestReplyMatch, NullWritable> {

    @Override
    public void reduce(IntWritable key, Iterable<WebTrafficRecord> values,
                       Context ctxt) throws IOException, InterruptedException {

        // The input is a set of WebTrafficRecords for each key,
        // the output should be the WebTrafficRecords for
        // all cases where the request and reply are matched
        // as having the same
        // Source IP/Source Port/Destination IP/Destination Port
        // and have occured within a 10 second window on the timestamp.

        // One thing to really remember, the Iterable element passed
        // from hadoop are designed as READ ONCE data, you will
        // probably want to copy that to some other data structure if
        // you want to iterate mutliple times over the data.
        // Using ArrayList to store all the requests and replies
        ArrayList<WebTrafficRecord> requests = new ArrayList<WebTrafficRecord>();
        ArrayList<WebTrafficRecord> replies = new ArrayList<WebTrafficRecord>();       
        // Run through all the elements in values
        for (WebTrafficRecord element : values) {
            // If the element already has a username, then it has been requested, add to replies
            if (element.getUserName() != null) {
                replies.add(new WebTrafficRecord(element));
            } else { // Otherwise, it has not been requested
                requests.add(new WebTrafficRecord(element));
            }
        }
        // Go through both lists to find the match
        for (int i = 0; i < requests.size(); i++) {
            WebTrafficRecord request = requests.get(i);
            for (int j = 0; j < replies.size(); j++) {
                WebTrafficRecord reply = replies.get(j);
                // Check if two elements can match
                if (request.tupleMatches(reply)) {
                    // Check if the difference in timestamp is smaller than 10L
                    if (10L > Math.abs(request.getTimestamp() - reply.getTimestamp())) {
                        RequestReplyMatch match = new RequestReplyMatch(request, reply);
                        // New match found and write to the context
                        ctxt.write(match, NullWritable.get());
                        // ctxt.write should be RequestReplyMatch and a NullWriteable
                    }
                }
            }
        }        
    }
}
