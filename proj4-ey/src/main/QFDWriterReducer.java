import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;
import java.io.ObjectOutputStream;
import java.util.HashSet;
import org.apache.hadoop.fs.*;

import java.io.IOException;

public class QFDWriterReducer extends Reducer<WTRKey, RequestReplyMatch, NullWritable, NullWritable> {

    @Override
    public void reduce(WTRKey key, Iterable<RequestReplyMatch> values,
                       Context ctxt) throws IOException, InterruptedException {

        // The input will be a WTR key and a set of matches.

        // You will want to open the file named
        // "qfds/key.getName()/key.getName()_key.getHashBytes()"
        // using the FileSystem interface for Hadoop.

        // EG, if the key's name is srcIP and the hash is 2BBB,
        // the filename should be qfds/srcIP/srcIP_2BBB

        // Some useful functionality:

        // FileSystem.get(ctxt.getConfiguration())
        // gets the interface to the filesysstem
        // new Path(filename) gives a path specification
        // hdfs.create(path, true) will create an
        // output stream pointing to that file

        // Using a hashSet to store all the matches
        HashSet<RequestReplyMatch> matches = new HashSet<RequestReplyMatch>();

        for (RequestReplyMatch value : values) {
            // Since every value corresponds to a match
            RequestReplyMatch match = new RequestReplyMatch(value);
            matches.add(match);
        }
        QueryFocusedDataSet qfds = new QueryFocusedDataSet(key.getName(), key.getHashBytes(), matches);
        try {
            String name = key.getName();
            String hashBytes = key.getHashBytes();
            Path path = new Path("qfds/" + name + "/" + name + "_" + hashBytes);
            // Initialize and interface to the filesystem
            FileSystem file_System = FileSystem.get(ctxt.getConfiguration());
            // So the dataputstream and the objectoutput stream is going to be:
            FSDataOutputStream data_output = file_System.create(path, true);
            ObjectOutputStream object_output = new ObjectOutputStream(data_output);
            object_output.writeObject(qfds);
            // Close the stream once write the data set
            object_output.close();
            data_output.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
