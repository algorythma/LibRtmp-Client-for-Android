package net.butterflytv.rtmp_client;

public interface RTMPCallback {
    void dataCallback (byte[] buffer);
    void functionCallback ();
}
