import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.*;
import javax.xml.bind.DatatypeConverter;
import java.util.*;
import java.io.ObjectInputStream;
import java.io.IOException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.nio.charset.StandardCharsets;
public class TotalFailMapper extends Mapper<LongWritable, Text, WTRKey,
                                            RequestReplyMatch> {

    private MessageDigest messageDigest;
    @Override
    public void setup(Context ctx) throws InterruptedException, IOException {
        // You probably need to do the same setup here you did
        // with the QFD writer
        super.setup(ctx);
        try {
            messageDigest = MessageDigest.getInstance("SHA-1");
        } catch (NoSuchAlgorithmException e) {
            throw new RuntimeException("SHA-1 algorithm not available");
        }
        messageDigest.update(HashUtils.SALT.getBytes(StandardCharsets.UTF_8));
    }

    @Override
    public void map(LongWritable lineNo, Text line, Context ctxt)
            throws IOException, InterruptedException {

        // The value in the input for the key/value pair is a Tor IP.
        // You need to then query that IP's source QFD to get
        // all cookies from that IP,
        // query the cookie QFDs to get all associated requests
        // which are by those cookies, and store them in a torusers QFD
        String srcIp = line.toString();
        try {
            // Get the hashString with the digested message
            MessageDigest md = HashUtils.cloneMessageDigest(messageDigest);
            md.update(srcIp.getBytes(StandardCharsets.UTF_8));
            byte[] hash = md.digest();
            byte[] hashBytes = Arrays.copyOf(hash, HashUtils.NUM_HASH_BYTES);
            String hashString = DatatypeConverter.printHexBinary(hashBytes);
            // Construct the path
            Path path = new Path("qfds/srcIP/srcIP_" + hashString);
            // Get the interface
            FileSystem file_System = FileSystem.get(ctxt.getConfiguration());
            FSDataInputStream data_input = file_System.open(path);
            ObjectInputStream object_input = new ObjectInputStream(data_input);
            QueryFocusedDataSet qfds = (QueryFocusedDataSet) object_input.readObject();
            object_input.close();
            HashSet<String> cookies = new HashSet<String>();
            // Iterate through all the matches in the dataset
            for (RequestReplyMatch match_element : qfds.getMatches()) {
                if (match_element.getSrcIp().equals(srcIp)) {
                    String new_match = match_element.getCookie();
                    cookies.add(new_match);
                }
            }
            // Iterate through the cookie set
            for (String cookie : cookies) {
                try {
                    MessageDigest md_cookie = HashUtils.cloneMessageDigest(messageDigest);
                    md_cookie.update(cookie.getBytes(StandardCharsets.UTF_8));
                    byte[] hash_cookie = md_cookie.digest();
                    byte[] hashBytes_cookie = Arrays.copyOf(hash_cookie, HashUtils.NUM_HASH_BYTES);
                    String hashString_cookie = DatatypeConverter.printHexBinary(hashBytes_cookie);
                    Path path2 = new Path("qfds/cookie/cookie_" + hashString_cookie);
                    // Initiate a new cycle
                    FileSystem file_System2 = FileSystem.get(ctxt.getConfiguration());
                    FSDataInputStream data_input2 = file_System.open(path2);
                    ObjectInputStream object_input2 = new ObjectInputStream(data_input2);
                    // Update the dataset if there is no error
                    qfds = (QueryFocusedDataSet) object_input2.readObject();
                    object_input.close();
                } catch (Exception e) {
                    e.printStackTrace();
                }
                // Iterate through the match set to find the match corresponding to the cookie
                for (RequestReplyMatch match_element : qfds.getMatches()) {
                    if (match_element.getCookie().equals(cookie)) {
                        MessageDigest md_match = HashUtils.cloneMessageDigest(messageDigest);
                        md_match.update(match_element.getUserName().getBytes(StandardCharsets.UTF_8));
                        byte[] hash_match = md_match.digest();
                        byte[] hashBytes_match = Arrays.copyOf(hash_match, HashUtils.NUM_HASH_BYTES);
                        String hashString_match = DatatypeConverter.printHexBinary(hashBytes_match);
                        WTRKey key_match = new WTRKey("torusers", hashString_match);
                        ctxt.write(key_match, match_element);
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
