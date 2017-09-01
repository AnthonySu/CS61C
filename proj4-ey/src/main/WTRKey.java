import org.apache.hadoop.io.WritableComparable;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.Objects;

public class WTRKey implements WritableComparable {

    private String name;
    private String hashBytes;

    public WTRKey() {}

    public WTRKey(String name, String hashBytes) {
        this.name = name;
        this.hashBytes = hashBytes;
    }

    public String getName() {
        return name;
    }

    public String getHashBytes() {
        return hashBytes;
    }

    @Override
    public void write(DataOutput dataOutput) throws IOException {
        dataOutput.writeUTF(name);
        dataOutput.writeUTF(hashBytes);
    }

    @Override
    public void readFields(DataInput dataInput) throws IOException {
        name = dataInput.readUTF();
        hashBytes = dataInput.readUTF();
    }

    @Override
    public int compareTo(Object o) {
        if (!(o instanceof WTRKey)) {
            throw new RuntimeException("Attempted comparison between WTRKey instance and non WTRKey instance");
        }

        WTRKey other = (WTRKey) o;
        int nameComparison = this.name.compareTo(other.name);
        if (nameComparison != 0) {
            return nameComparison;
        }
        // Names are identical, so we compare by hashBytes
        return this.hashBytes.compareTo(other.hashBytes);
    }

    @Override
    public boolean equals(Object o) {
        if (o == null) {
            return false;
        } else if (o == this) {
            return true;
        } else if (!(o instanceof WTRKey)) {
            return false;
        }
        WTRKey other = (WTRKey) o;

        return Objects.equals(this.name, other.name) &&
               Objects.equals(this.hashBytes, other.hashBytes);
    }

    @Override
    public int hashCode() {
        return Objects.hash(name, hashBytes);
    }
}
