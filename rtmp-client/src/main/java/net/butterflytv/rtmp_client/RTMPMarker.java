package net.butterflytv.rtmp_client;

import java.util.Arrays;

public class RTMPMarker {
    private String type;
    private double uid;
    private double retryIndex;
    private byte[] data;
    private boolean valid;

    RTMPMarker (byte[] type, double uid, double retryIndex, byte[] data, boolean valid) {
        if (type != null)
            this.type = new String (type);
        else
            this.type = new String ();
        this.uid = uid;
        this.retryIndex = retryIndex;
        if (data != null)
            this.data = Arrays.copyOf (data, data.length);
        else
            this.data = null;
        this.valid = valid;
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

    public boolean isValid () { return this.valid; }

    public void setValid (boolean valid) { this.valid = valid; }
}