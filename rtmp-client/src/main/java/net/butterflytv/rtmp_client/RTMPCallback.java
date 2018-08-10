package net.butterflytv.rtmp_client;

public interface RTMPCallback {
    void dataCallback (RTMPMarker marker);
    void functionCallback ();
}
