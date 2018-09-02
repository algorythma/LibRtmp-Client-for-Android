#include <malloc.h>
#include <string.h>
#include "librtmp-jni.h"
#include "rtmp.h"
//
// Created by faraklit on 01.01.2016.
//


#define  LOG_TAG    "someTag"

#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

//RTMP *rtmp = NULL;

pthread_key_t current_jni_env;

JNIEXPORT jlong JNICALL
Java_net_butterflytv_rtmp_1client_RtmpClient_nativeAlloc(JNIEnv *env, jobject instance) {
    RTMP *rtmp = RTMP_Alloc();
    if (rtmp == NULL) {
        return -1;
    }
    cachedRtmpClientObj = (*env)->NewGlobalRef (env, instance);
    return (long)rtmp;
}

/*
 * Class:     net_butterflytv_rtmp_client_RtmpClient
 * Method:    open
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_net_butterflytv_rtmp_1client_RtmpClient_nativeOpen
        (JNIEnv * env, jobject thiz, jstring url_, jboolean isPublishMode, jlong rtmpPointer) {

    const char *url = (*env)->GetStringUTFChars(env, url_, 0);
    RTMP *rtmp = (RTMP *) rtmpPointer;
   // rtmp = RTMP_Alloc();
    if (rtmp == NULL) {
        return -1;
    }

	RTMP_Init(rtmp);
	int ret = RTMP_SetupURL(rtmp, url);

    if (!ret) {
        RTMP_Free(rtmp);
        return -2;
    }
    if (isPublishMode) {
        RTMP_EnableWrite(rtmp);
    }

	ret = RTMP_Connect(rtmp, NULL);
    if (!ret) {
        RTMP_Free(rtmp);
        return -3;
    }
	ret = RTMP_ConnectStream(rtmp, 0);

    if (!ret) {
        return -4;
    }
    (*env)->ReleaseStringUTFChars(env, url_, url);
    return 1;
}



/*
 * Class:     net_butterflytv_rtmp_client_RtmpClient
 * Method:    read
 * Signature: ([CI)I
 */
JNIEXPORT jint JNICALL Java_net_butterflytv_rtmp_1client_RtmpClient_nativeRead
        (JNIEnv * env, jobject thiz, jbyteArray data_, jint offset, jint size, jlong rtmpPointer) {

    RTMP *rtmp = (RTMP *) rtmpPointer;
    jclass rtmpClientClass;
    jfieldID fid;

    if (rtmp == NULL) {
        throwIOException(env, "First call open function");
    }
    int connected = RTMP_IsConnected(rtmp);
    if (!connected) {
        throwIOException(env, "Connection to server is lost");
    }

    memset (&markerInfo, 0, sizeof (RTMPMarkerInfo));

    char* data = malloc(size*sizeof(char));

    int readCount = RTMP_Read(rtmp, data, size);

    if (markerInfo.valid) {
//        jbyteArray type = (*env)->NewByteArray (env, strlen (markerInfo.type) + 1);
//        (*env)->SetByteArrayRegion(env, type, 0, strlen (markerInfo.type) + 1, (const jbyte *)markerInfo.type);

        jbyteArray type = (*env)->NewByteArray (env, strlen (markerInfo.type) + 1);
        (*env)->SetByteArrayRegion(env, type, 0, strlen (markerInfo.type) + 1, (const jbyte *)markerInfo.type);

        jbyteArray byteArr = (*env)->NewByteArray(env, strlen (markerInfo.data) + 1);
        (*env)->SetByteArrayRegion(env, byteArr, 0, strlen (markerInfo.data) + 1, (const jbyte *)markerInfo.data);


        jclass markerClass = (*env)->FindClass (env, "net/butterflytv/rtmp_client/RTMPMarker");
        __android_log_print (ANDROID_LOG_INFO, "RTMP_CLIENT_ANDROID_LOG",
                             "%s: env: %p, markerClass: %p",
                             __FUNCTION__, *env, markerClass);
        jmethodID markerConstructorID = (*env)->GetMethodID (env, markerClass, "<init>", "([BDD[BZ)V");
        __android_log_print (ANDROID_LOG_INFO, "RTMP_CLIENT_ANDROID_LOG",
                             "%s: env: %p, markerConstructorID: %p",
                             __FUNCTION__, *env, markerConstructorID);
        jobject newMrkrObj = (*env)->NewObject (env, markerClass, markerConstructorID, type,
                                                markerInfo.uid, markerInfo.index, byteArr, JNI_TRUE);
        __android_log_print (ANDROID_LOG_INFO, "RTMP_CLIENT_ANDROID_LOG",
                             "%s: env: %p, newMrkrObj: %p", __FUNCTION__, *env, newMrkrObj);

        rtmpClientClass = (*env)->GetObjectClass (env, thiz);
        __android_log_print (ANDROID_LOG_INFO, "RTMP_CLIENT_ANDROID_LOG",
                             "%s: env: %p, rtmpClientClass: %p",
                             __FUNCTION__, *env,rtmpClientClass);
        fid = (*env)->GetFieldID (env, rtmpClientClass, "mrkr", "Lnet/butterflytv/rtmp_client/RTMPMarker;");
        if (fid ==  NULL)
          __android_log_print (ANDROID_LOG_ERROR, "RTMP_CLIENT_ANDROID_LOG", "%s: "
                  "RTMPMarker GetFieldID is NULL", __FUNCTION__);

        (*env)->SetObjectField (env, thiz, fid, newMrkrObj);

        (*env)->DeleteLocalRef (env, type);
        (*env)->DeleteLocalRef (env, byteArr);
        (*env)->DeleteLocalRef (env, newMrkrObj);
        markerInfo.valid = 0;
        free (markerInfo.type);
        free (markerInfo.data);
    }
    if (readCount > 0) {
        (*env)->SetByteArrayRegion(env, data_, offset, readCount, data);  // copy
    }
    free(data);
    if (readCount == 0) {
        return -1;
    }
 	return readCount;
}

/*
 * Class:     net_butterflytv_rtmp_client_RtmpClient
 * Method:    write
 * Signature: ([CI)I
 */
JNIEXPORT jint JNICALL Java_net_butterflytv_rtmp_1client_RtmpClient_nativeWrite
        (JNIEnv * env, jobject thiz, jcharArray data, jint size, jlong rtmpPointer) {

    RTMP *rtmp = (RTMP *) rtmpPointer;
    if (rtmp == NULL) {
        throwIOException(env, "First call open function");
    }

    int connected = RTMP_IsConnected(rtmp);
    if (!connected) {
        throwIOException(env, "Connection to server is lost");
    }

    return RTMP_Write(rtmp, data, size);
}

/*
 * Class:     net_butterflytv_rtmp_client_RtmpClient
 * Method:    seek
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_net_butterflytv_rtmp_1client_RtmpClient_seek
        (JNIEnv * env, jobject thiz, jint seekTime) {

    return 0;
}

/*
 * Class:     net_butterflytv_rtmp_client_RtmpClient
 * Method:    pause
 * Signature: (I)I
 */
JNIEXPORT bool JNICALL Java_net_butterflytv_rtmp_1client_RtmpClient_nativePause
        (JNIEnv * env, jobject thiz, jboolean pause, jlong rtmpPointer) {

    RTMP *rtmp = (RTMP *) rtmpPointer;
    if (rtmp == NULL) {
        throwIOException(env, "First call open function");
    }

    int DoPause = 0;
    if (pause == JNI_TRUE) {
        DoPause = 1;
    }
    return RTMP_Pause(rtmp, DoPause);
}

/*
 * Class:     net_butterflytv_rtmp_client_RtmpClient
 * Method:    close
 * Signature: ()I
 */
JNIEXPORT void JNICALL Java_net_butterflytv_rtmp_1client_RtmpClient_nativeClose
        (JNIEnv * env, jobject thiz, jlong rtmpPointer) {

    RTMP *rtmp = (RTMP *) rtmpPointer;
    (*env)->DeleteGlobalRef (env, cachedRtmpClientObj);
    if (rtmp != NULL) {
        RTMP_Close(rtmp);
        RTMP_Free(rtmp);
    }
}


JNIEXPORT bool JNICALL
Java_net_butterflytv_rtmp_1client_RtmpClient_nativeIsConnected(JNIEnv *env, jobject instance, jlong rtmpPointer)
{
    RTMP *rtmp = (RTMP *) rtmpPointer;
    if (rtmp == NULL) {
        return false;
    }
     int connected = RTMP_IsConnected(rtmp);
     if (connected) {
        return true;
     }
     else {
        return false;
     }
}

jint throwIOException (JNIEnv *env, char *message )
{
    jclass Exception = (*env)->FindClass(env, "java/io/IOException");
    return (*env)->ThrowNew(env, Exception, message);
}

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;

    java_vm = vm;

    if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        __android_log_print (ANDROID_LOG_ERROR, "RTMPCLIENT", "Could not retrieve JNIEnv");
        return 0;
    }

    pthread_key_create (&current_jni_env, detach_current_thread);

    return JNI_VERSION_1_4;
}

void forwardDataCBToApp (RTMPMarkerInfo *mInfo) {
    JNIEnv *env;
    jclass rtmpClientClass;
    jmethodID mid;

    env = get_jni_env();

    jbyteArray type = (*env)->NewByteArray (env, strlen (mInfo->type) + 1);
    (*env)->SetByteArrayRegion(env, type, 0, strlen (mInfo->type) + 1, (const jbyte *)mInfo->type);

    jbyteArray byteArr = (*env)->NewByteArray(env, strlen (mInfo->data) + 1);
    (*env)->SetByteArrayRegion(env, byteArr, 0, strlen (mInfo->data) + 1, mInfo->data);

    rtmpClientClass = (*env)->FindClass (env, "net/butterflytv/rtmp_client/RtmpClient");
    __android_log_print (ANDROID_LOG_INFO, "RTMP_CLIENT_ANDROID_LOG",
                         "%s: env: %p, rtmpClientClass: %p / %p, using NON_CACHEDENV",
                         __FUNCTION__, *env, cachedRtmpClientObj,
                         (*env)->GetObjectClass (env, cachedRtmpClientObj));
    mid = (*env)->GetMethodID (env, rtmpClientClass, "RtmpDataCallback", "([BDD[B)V");
    (*env)->CallVoidMethod (env, cachedRtmpClientObj, mid, type, mInfo->uid, mInfo->index, byteArr);

    (*env)->DeleteLocalRef (env, type);
    (*env)->DeleteLocalRef (env, byteArr);

}

void forwardFnctCBToApp () {
    JNIEnv *env;
    jclass rtmpClientClass;
    jmethodID mid;

    env = get_jni_env();
    __android_log_print (ANDROID_LOG_INFO, "RTMP_CLIENT_ANDROID_LOG",
                         "%s: env: %p", __FUNCTION__, *env);

    rtmpClientClass = (*env)->FindClass (env, "net/butterflytv/rtmp_client/RtmpClient");
    mid = (*env)->GetMethodID (env, rtmpClientClass, "RtmpFunctionCallback", "()V");
    (*env)->CallVoidMethod (env, cachedRtmpClientObj, mid);
}