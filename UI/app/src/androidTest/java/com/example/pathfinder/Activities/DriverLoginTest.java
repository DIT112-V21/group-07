package com.example.pathfinder.Activities;

import androidx.test.espresso.action.TypeTextAction;
import androidx.test.espresso.intent.Intents;
import androidx.test.ext.junit.rules.ActivityScenarioRule;

import com.example.pathfinder.R;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;

import static androidx.test.espresso.Espresso.onView;
import static androidx.test.espresso.action.ViewActions.click;
import static androidx.test.espresso.assertion.ViewAssertions.matches;
import static androidx.test.espresso.intent.matcher.IntentMatchers.hasComponent;
import static androidx.test.espresso.matcher.ViewMatchers.isDisplayed;
import static androidx.test.espresso.matcher.ViewMatchers.withId;
import static androidx.test.espresso.matcher.ViewMatchers.withText;


public class DriverLoginTest {


    /**
     * Setup the rules for the different tests run in this class. Rules are destroyed at the end of each test
     * and created at the beginning of each test.
     * In this case, we define the rules as coming from the DriverLogin.class (instantiate every element of that class) and setup the activity for testing.
     */
    @Rule
    public ActivityScenarioRule <DriverLogin> activity = new ActivityScenarioRule<>(DriverLogin.class);



    /**
     * Custom setup run before each test. In this case, we initialise the Intents so it starts recording the interaction with the UI.
     * See doc for Intents to learn more.
     * We also ask to wait 3 seconds between each test, so toasts are not overlapping and make tests fail.
     */
    @Before
    public void setUp() {
        Intents.init();
        try {
            Thread.sleep(3000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    /**
     * Custom function that run after each test. In this case, we need to release the Intents since it is demanded by the API.
     */
    @After
    public void reset(){
        Intents.release();

    }


    /**
     * Tests if when entering a registered user, a matching password and clicking on the button, the user gets redirected to the dashboard.
     */
    @Test
    public void connectionSucceeded_changeActivityForDashboard() {
        onView(withId(R.id.usernameInput)).perform(new TypeTextAction("Alex"));
        onView(withId(R.id.passwordInput)).perform(new TypeTextAction("1234"));
        onView(withId(R.id.connectButton)).perform(click());

        Intents.intended(hasComponent(DashboardActivity.class.getName()));

    }

    //The tests below are not working on an Android API30 but are working well on the API 29. It is a reported bug that has not been yet corrected it seems.
    //We might need to deactivate them or use a API 29 for the CI.

    /**
     * Tests if when entering a registered user but un non matching password and clicking on the button, the user gets a toast with a connection failed message.
     */
    @Test
    public void invalidPassword_getNotificationThatConnectionFailed() {
        onView(withId(R.id.usernameInput)).perform(new TypeTextAction("Alex"));
        onView(withId(R.id.passwordInput)).perform(new TypeTextAction("123"));
        onView(withId(R.id.connectButton)).perform(click());

        onView(withText(R.string.connection_failed))
                .inRoot(new ToastMatcher("Is a Toast"))
                        .check(matches(isDisplayed()));

    }

    /**
     * Tests if when entering a unregistered user and a password and clicking on the button, the user gets a toast with a connection failed message.
     */
    @Test
    public void unregisteredUsername_getNotificationThatConnectionFailed() {
        onView(withId(R.id.usernameInput)).perform(new TypeTextAction("George"));
        onView(withId(R.id.passwordInput)).perform(new TypeTextAction("123"));
        onView(withId(R.id.connectButton)).perform(click());

        onView(withText(R.string.connection_failed))
                .inRoot(new ToastMatcher("Is a Toast"))
                .check(matches(isDisplayed()));

    }

    /**
     * Tests if when leaving the username field empty, giving a password and clicking on the button, the user gets a toast with a connection failed message.
     */
    @Test
    public void emptyUsername_getNotificationThatConnectionFailed() {
        String empty = "";
        onView(withId(R.id.usernameInput)).perform(new TypeTextAction(empty));
        onView(withId(R.id.passwordInput)).perform(new TypeTextAction("123"));
        onView(withId(R.id.connectButton)).perform(click());

        onView(withText(R.string.connection_failed))
                .inRoot(new ToastMatcher("Is a Toast"))
                .check(matches(isDisplayed()));

    }

    /**
     * Tests if when leaving the username field empty, giving a password and clicking on the button, the user gets a toast with a connection failed message.
     */
    @Test
    public void emptyPassword_getNotificationThatConnectionFailed() {
        String empty = "";
        onView(withId(R.id.usernameInput)).perform(new TypeTextAction("Alex"));
        onView(withId(R.id.passwordInput)).perform(new TypeTextAction(empty));
        onView(withId(R.id.connectButton)).perform(click());

        onView(withText(R.string.connection_failed))
                .inRoot(new ToastMatcher("Is a Toast"))
                .check(matches(isDisplayed()));

    }

}