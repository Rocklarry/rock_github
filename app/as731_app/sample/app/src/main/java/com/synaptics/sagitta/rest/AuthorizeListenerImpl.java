package com.synaptics.sagitta.rest;

import android.util.Log;
import com.amazon.identity.auth.device.AuthError;
import com.amazon.identity.auth.device.api.authorization.AuthCancellation;
import com.amazon.identity.auth.device.api.authorization.AuthorizeListener;
import com.amazon.identity.auth.device.api.authorization.AuthorizeResult;


public class AuthorizeListenerImpl extends AuthorizeListener {

    private String TAG = "==rock===  AuthorizeListenerImpl";

    /* Authorization was completed successfully. */
    public void onSuccess(final AuthorizeResult authorizeResult) {
        final String authorizationCode = authorizeResult.getAuthorizationCode();
        final String redirectUri = authorizeResult.getRedirectURI();
        final String clientId = authorizeResult.getClientId();
        Log.d(TAG, "AVS authorization succeed");
        Log.d(TAG, "authorizationCode: "+ authorizationCode);
        Log.d(TAG, "redirectUri: " + redirectUri);
        Log.d(TAG, "clientId: " + clientId);
    }

    /* There was an error during the attempt to authorize the application. */
    @Override
    public void onError(final AuthError authError) {
        Log.d(TAG, "AVS authorization failed");
    }

    /* Authorization was cancelled before it could be completed. */
    @Override
    public void onCancel(final AuthCancellation authCancellation) {
        Log.d(TAG, "AVS authorization failed");
    }
}