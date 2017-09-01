import org.apache.hadoop.io.Writable;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import java.io.Serializable;
import java.util.Objects;

public class RequestReplyMatch implements Writable, Serializable {
    private WebTrafficRecord request;
    private WebTrafficRecord reply;

    // Need null constructor for deserialization
    public RequestReplyMatch(){
    }

    public RequestReplyMatch(WebTrafficRecord request, WebTrafficRecord reply) {
        this.request = request;
        this.reply = reply;
    }

    public RequestReplyMatch(RequestReplyMatch other) {
        this.request = new WebTrafficRecord(other.request);
        this.reply = new WebTrafficRecord(other.reply);
    }

    public WebTrafficRecord getRequest() {
        return request;
    }

    public WebTrafficRecord getReply() {
        return reply;
    }

    @Override
    public void write(DataOutput dataOutput) throws IOException {
        request.write(dataOutput);
        reply.write(dataOutput);
    }

    @Override
    public void readFields(DataInput dataInput) throws IOException {
        request = new WebTrafficRecord();
        request.readFields(dataInput);
        reply = new WebTrafficRecord();
        reply.readFields(dataInput);
    }

    @Override
    public String toString() {
	return String.format("Request\n%s\nReply\n%s", request.toString(), reply.toString());
    }

    @Override
    public boolean equals(Object o) {
        if (o == null) {
            return false;
        } else if (o == this) {
            return true;
        } else if (!(o instanceof RequestReplyMatch)) {
            return false;
        }
        RequestReplyMatch other = (RequestReplyMatch) o;

        return this.request.equals(other.request) && this.reply.equals(other.reply);
    }

    @Override
    public int hashCode() {
        return Objects.hash(request, reply);
    }

    public String getSrcIp() {
        return this.request.getSrcIp();
    }

    public int getSrcPort() {
        return this.request.getSrcPort();
    }

    public String getDestIp() {
        return this.request.getDestIp();
    }

    public int getDestPort() {
        return this.request.getDestPort();
    }

    public String getUserName() {
        return this.reply.getUserName();
    }

    public String getCookie(){
        return this.request.getCookie();
    }
}
