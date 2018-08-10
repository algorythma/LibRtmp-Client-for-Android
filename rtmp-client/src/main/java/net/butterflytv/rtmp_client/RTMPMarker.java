package net.butterflytv.rtmp_client;

import java.util.Arrays;

public class RTMPMarker {
    private final String type;
    private final double uid;
    private final double retryIndex;
    private final byte[] data;

    RTMPMarker (byte[] type, double uid, double retryIndex, byte[] data) {
        this.type = new String (type);
        this.uid = uid;
        this.retryIndex = retryIndex;
        this.data = Arrays.copyOf (data, data.length);
    }

    public String getType() {
        return this.type;
    }

    public double getUid() {
        return this.uid;
    }

    public double getRetryIndex () {
        return this.retryIndex;
    }

    public byte[] getData () {
        return this.data;
    }
}