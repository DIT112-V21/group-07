package com.example.pathfinder.Activities;

import android.os.IBinder;
import android.view.WindowManager;

import androidx.test.espresso.Root;

import org.hamcrest.CustomTypeSafeMatcher;

public class ToastMatcher extends CustomTypeSafeMatcher <Root> {


    public ToastMatcher(String description) {
        super(description);
    }


    // Author : QA automated
    // Website : http://www.qaautomated.com/2016/01/how-to-test-toast-message-using-espresso.html
    /**
     * Check if the root type is a Toast and if the window the root is attached to belongs to this application.
     * @param root -> root view in the app
     * @return true if the root if of type Toast and belong to a window of the app.
     */
    @Override
    protected boolean matchesSafely(Root root) {
        int type = root.getWindowLayoutParams().get().type;
        if (type == WindowManager.LayoutParams.TYPE_TOAST){
            IBinder window = root.getDecorView().getWindowToken();
            IBinder app = root.getDecorView().getApplicationWindowToken();
            return window == app;
        }
        return false;
    }

}
