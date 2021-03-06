package net.butterflytv.rtmp_client;

import android.util.Log;

import java.io.IOException;
import java.util.Arrays;

/**
 * Created by faraklit on 01.01.2016.
 */
public class RtmpClient {

    public RTMPCallback rtmpCallback;
    public RTMPMarker mrkr;
    static {
        System.loadLibrary("rtmp-jni");
    }

    public RtmpClient () {}
    public RtmpClient (RTMPCallback rtmpCallback) {
        this.rtmpCallback = rtmpCallback;
    }

    public final static int OPEN_SUCCESS = 1;
    private long rtmpPointer = 0;

    public static class RtmpIOException extends IOException {

        /**
         * it means there is a problem in memory allocation
         */
        public final static int OPEN_ALLOC = -1;

        /**
         * it means there is a problem in setting url, check the rtmp url
         */
        public final static int OPEN_SETUP_URL = -2;

        /**
         *  it means there is a problem in connecting to the rtmp server,
         *  check there is an active network connection,
         *  check rtmp server is running,
         */
        public final static int OPEN_CONNECT = -3;

        /**
         *  it means there is a problem in connecting stream
         */
        public final static int OPEN_CONNECT_STREAM = -4;


        public final int errorCode;

        public RtmpIOException(int errorCode) {
            this.errorCode = errorCode;
        }

    }

    public void open(String url, boolean isPublishMode) throws RtmpIOException {
        rtmpPointer = nativeAlloc();
        int result = nativeOpen(url, isPublishMode, rtmpPointer);
        if (result != OPEN_SUCCESS) {
            rtmpPointer = 0;
            throw new RtmpIOException(result);
        }
        mrkr = new RTMPMarker (null, -1, -1, null, false);
    }

    private native long nativeAlloc();

    /**
     * opens the rtmp url
     * @param url
     * url of the stream
     * @param isPublishMode
     * if this is an publication it is true,
     * if connection is for getting stream it is false
     * @return return a minus value if it fails
     * returns

     *
     * returns {@link #OPEN_SUCCESS} if it is successful, throws RtmpIOException if it is failed
     */
    private native int nativeOpen(String url, boolean isPublishMode, long rtmpPointer);

    /**
     * read data from rtmp connection
     *
     * @param data
     * buffer that will be filled
     * @param offset
     * offset to read data
     * @param size
     * size of the data to be reat
     * @return
     * number of bytes to be read
     *
     * if it returns -1, it means stream is complete
     *  and close function can be called.
     *
     *
     *  @throws IOException if connection is not opened or connection to server is lost
     *
     */
    public int read(byte[] data, int offset, int size) throws IOException {
        int ret = 0;

        ret = nativeRead(data, offset, size, rtmpPointer);
//        if (mrkr.isValid()) {
//            rtmpCallback.dataCallback(mrkr);
//            mrkr.setValid (false);
//        }
        return ret;
    }

    public void markerToastDisplay () {
        if (mrkr.isValid()) {
            rtmpCallback.dataCallback(mrkr);
            mrkr.setValid(false);
        }
    }

    public int read(byte[] data, int offset, int size, boolean [] isMarker) throws IOException {
        int ret = 0;

        ret = nativeRead(data, offset, size, rtmpPointer);
        if (mrkr.isValid()) {
            isMarker[0] = true;
//            rtmpCallback.dataCallback(mrkr);
//            mrkr.setValid (false);
        }
        return ret;
    }
    private native int nativeRead(byte[] data, int offset, int size, long rtmpPointer) throws IOException;

    /**
     *
     * @param data
     * @return number of bytes written
     * @throws IOException if connection is not opened or connection to server is lost
     */
    public int write(byte[] data) throws IOException {
        return nativeWrite(data, rtmpPointer);
    }

    private native int nativeWrite(byte[] data, long rtmpPointer) throws IOException;


    /**
     * @param pause
     * if pause is true then stream is going to be paused
     *
     * If pause is false, it unpauses the stream and it is ready to to play again
     *
     * @return true if it is successfull else returns false
     */
    public boolean pause(boolean pause) {
        return nativePause(pause, rtmpPointer);
    }
    private native boolean nativePause(boolean pause, long rtmpPointer);



    /**
     *
     * @return true if it is connected
     * false if it is not connected
     */
    public boolean isConnected() {
        return nativeIsConnected(rtmpPointer);
    }

    private native boolean nativeIsConnected(long rtmpPointer);

    /**
     *
     * closes the connection. Dont forget to call
     */
    public void close() {
        nativeClose(rtmpPointer);
    }

    private native void nativeClose(long rtmpPointer);

    public void RtmpDataCallback (byte[] type, double uid, double index, byte[] buffer) {
        RTMPMarker marker = new RTMPMarker (type, uid, index, buffer, true);
        rtmpCallback.dataCallback(marker);
    }

    public void RtmpFunctionCallback () {
        rtmpCallback.functionCallback();
    }
}
