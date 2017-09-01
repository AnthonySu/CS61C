import org.apache.hadoop.io.LongWritable;
import java.security.NoSuchAlgorithmException;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Mapper;
import javax.xml.bind.DatatypeConverter;
import java.util.Arrays;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
public class QFDWriterMapper extends Mapper<RequestReplyMatch,
				     NullWritable, WTRKey,
				     RequestReplyMatch> {
    private MessageDigest messageDigest;


    // For the Query Focused Dataset, we take the key we are using,
    // hash it with a cryptographic hash function, and only use
    // the HashUtils.NUM_HASH_BYTES.

    // We start by creating a messageDigest instance, and preseed it
    // with the salt also contained in HashUtils.  This is so that
    // nobody can predict which names map to which locations
    @Override
    public void setup(Context ctxt) throws IOException, InterruptedException {
        super.setup(ctxt);
        try {
            messageDigest = MessageDigest.getInstance("SHA-1");
        } catch (NoSuchAlgorithmException e) {
            // SHA-1 is required on all Java platforms, so this
            // should never occur
            throw new RuntimeException("SHA-1 algorithm not available");
        }
        // Now we are adding the salt...
        messageDigest.update(HashUtils.SALT.getBytes(StandardCharsets.UTF_8));
    }

    @Override
	public void map(RequestReplyMatch record, NullWritable ignore, Context ctxt)
	        throws IOException, InterruptedException {

        // This is the key mapping function.  You should
        // have the outputs be WTRKey/RequestReplyMatch pairs
        // since this dictates how to go to a particular
        // QFD

        // As an example, to dispatch for the "srcIP", the
        // hash should be

            // MessageDigest md = HashUtils.cloneMessageDigest(messageDigest);
            // md.update(record.getSrcIp().getBytes(StandardCharsets.UTF_8));
            // byte[] hash = md.digest();
            // byte[] hashBytes = Arrays.copyOf(hash, HashUtils.NUM_HASH_BYTES);
            // String hashString = DatatypeConverter.printHexBinary(hashBytes);
            // WTRKey key = new WTRKey("srcIP", hashString);
            // ctxt.write(key, record);

        // You need to do srcIP, destIP, and cookie QFD dispatch, and should
        // be able to use a much more generic structure
        MessageDigest md = HashUtils.cloneMessageDigest(messageDigest);
        md.update(record.getSrcIp().getBytes(StandardCharsets.UTF_8));
        byte[] hash = md.digest();
        byte[] hashBytes = Arrays.copyOf(hash, HashUtils.NUM_HASH_BYTES);
        String hashString = DatatypeConverter.printHexBinary(hashBytes);
        WTRKey key = new WTRKey("srcIP", hashString);
        ctxt.write(key, record);
        // destIP
        MessageDigest md_dest = HashUtils.cloneMessageDigest(messageDigest);
        md_dest.update(record.getDestIp().getBytes(StandardCharsets.UTF_8));
        byte[] hash_dest = md_dest.digest();
        byte[] hashBytes_dest = Arrays.copyOf(hash_dest, HashUtils.NUM_HASH_BYTES);
        String hashString_dest = DatatypeConverter.printHexBinary(hashBytes_dest);
        WTRKey key_dest = new WTRKey("destIP", hashString_dest);
        ctxt.write(key_dest, record);
        // cookie
        MessageDigest md_cookie = HashUtils.cloneMessageDigest(messageDigest);
        md_cookie.update(record.getCookie().getBytes(StandardCharsets.UTF_8));
        byte[] hash_cookie = md_cookie.digest();
        byte[] hashBytes_cookie = Arrays.copyOf(hash_cookie, HashUtils.NUM_HASH_BYTES);
        String hashString_cookie = DatatypeConverter.printHexBinary(hashBytes_cookie);
        WTRKey key_cookie = new WTRKey("cookie", hashString_cookie);
        ctxt.write(key_cookie, record);
    }
}
