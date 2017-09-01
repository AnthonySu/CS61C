import org.apache.hadoop.io.Writable;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import java.io.Serializable;
import java.util.*;

public class WebTrafficRecord implements Writable, Serializable {
    private static final int TIMESTAMP_IDX = 0;
    private static final int SOURCE_IP_IDX = 1;
    private static final int SOURCE_PORT_IDX = 2;
    private static final int DEST_IP_IDX = 3;
    private static final int DEST_PORT_IDX = 4;
    private static final int UNAME_COOKIE_IDX = 6;

    private long timestamp;
    private String srcIp;
    private int srcPort;
    private String destIp;
    private int destPort;
    private String userName;
    private String cookie;

    private WebTrafficRecord(long timestamp, String srcIp, int srcPort, String destIp, int destPort,
                             boolean isRequest, String userNameOrCookie) {
        this.timestamp = timestamp;
        this.srcIp = srcIp;
        this.srcPort = srcPort;
        this.destIp = destIp;
        this.destPort = destPort;

        if (isRequest) {
            cookie = userNameOrCookie;
            userName = null;
        } else {
            userName = userNameOrCookie;
            cookie = null;
        }
    }

    public static WebTrafficRecord newRequest(long timestamp, String srcIp, int srcPort, String destIp,
                                              int destPort, String cookie) {
        return new WebTrafficRecord(timestamp, srcIp, srcPort, destIp, destPort, true, cookie);
    }

    public static WebTrafficRecord newReply(long timestamp, String srcIp, int srcPort, String destIp,
                                            int destPort, String userName) {
        return new WebTrafficRecord(timestamp, srcIp, srcPort, destIp, destPort, false, userName);
    }

    public WebTrafficRecord() {}

    public WebTrafficRecord(WebTrafficRecord other) {
        this.timestamp = other.timestamp;
        this.srcIp = other.srcIp;
        this.srcPort = other.srcPort;
        this.destIp = other.destIp;
        this.destPort = other.destPort;
        this.cookie = other.cookie;
        this.userName = other.userName;
    }

    public long getTimestamp() {
        return timestamp;
    }

    public String getSrcIp() {
        return srcIp;
    }

    public int getSrcPort() {
        return srcPort;
    }

    public String getDestIp() {
        return destIp;
    }

    public int getDestPort() {
        return destPort;
    }

    public String getUserName() {
        return userName;
    }

    public String getCookie() {
        return cookie;
    }

    public static WebTrafficRecord parseFromLine(String line) {
        String[] tokens = line.split("\\t");
        assert tokens.length == 7;

        long timestamp = Long.parseLong(tokens[TIMESTAMP_IDX]);
        String srcIp = tokens[SOURCE_IP_IDX];
        int srcPort = Integer.parseInt(tokens[SOURCE_PORT_IDX]);
        String destIp = tokens[DEST_IP_IDX];
        int destPort = Integer.parseInt(tokens[DEST_PORT_IDX]);

        String lastToken = tokens[UNAME_COOKIE_IDX];
        if (lastToken.startsWith("cookie:")) {
            String cookie = lastToken.substring(lastToken.indexOf(':') + 1);
            return WebTrafficRecord.newRequest(timestamp, srcIp, srcPort, destIp, destPort, cookie);
        } else {
            String userName = lastToken.substring(lastToken.indexOf(':') + 1);
            return WebTrafficRecord.newReply(timestamp, srcIp, srcPort, destIp, destPort, userName);
        }
    }

    @Override
    public void write(DataOutput dataOutput) throws IOException {
        dataOutput.writeLong(timestamp);
        dataOutput.writeUTF(srcIp);
        dataOutput.writeInt(srcPort);
        dataOutput.writeUTF(destIp);
        dataOutput.writeInt(destPort);
        if (cookie != null) {
            dataOutput.writeInt(0);
            dataOutput.writeUTF(cookie);
        } else {
            dataOutput.writeInt(-1);
            dataOutput.writeUTF(userName);
        }
    }

    @Override
    public void readFields(DataInput dataInput) throws IOException {
        timestamp = dataInput.readLong();
        srcIp = dataInput.readUTF();
        srcPort = dataInput.readInt();
        destIp = dataInput.readUTF();
        destPort = dataInput.readInt();

        int type = dataInput.readInt();
        if (type == 0) {
            cookie = dataInput.readUTF();
            userName = null;
        } else {
            userName = dataInput.readUTF();
            cookie = null;
        }
    }

    public boolean tupleMatches(WebTrafficRecord other) {
        return this.getSrcIp().equals(other.getSrcIp()) &&
               this.getSrcPort() == other.getSrcPort() &&
               this.getDestIp().equals(other.getDestIp()) &&
               this.getDestPort() == other.getDestPort();
    }

    @Override
    public String toString() {
        if (cookie != null) {
            return String.format("Timestamp = %d\tsrcIP = %s\tsrcPort = %d\tdestIP = %s\tdestPort = %d\t" +
                                 "cookie = %s", timestamp, srcIp, srcPort, destIp, destPort, cookie);
        } else {
            return String.format("Timestamp = %d\tsrcIP = %s\tsrcPort = %d\tdestIP = %s\tdestPort = %d\t" +
                                 "userName = %s", timestamp, srcIp, srcPort, destIp, destPort, userName);
        }
    }

    @Override
    public boolean equals(Object o) {
        if (o == null) {
            return false;
        }
        else if (o == this) {
            return true;
        }
        else if (!(o instanceof WebTrafficRecord)) {
            return false;
        }
        WebTrafficRecord other = (WebTrafficRecord) o;

        return this.timestamp == other.timestamp &&
               Objects.equals(this.srcIp, other.srcIp) &&
               this.srcPort == other.srcPort &&
               Objects.equals(this.destIp, other.destIp) &&
               this.destPort == other.destPort &&
               Objects.equals(this.cookie, other.cookie) &&
               Objects.equals(this.userName, other.userName);
    }

    @Override
    public int hashCode() {
        return Objects.hash(srcIp, srcPort, destIp, destPort, timestamp);
    }

    public int matchHashCode() {
        return Objects.hash(srcIp, srcPort, destIp, destPort);
    }
}
