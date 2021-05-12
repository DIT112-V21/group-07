package com.example.pathfinder.Activities;

import androidx.test.espresso.action.TypeTextAction;
import androidx.test.espresso.intent.Intents;
import androidx.test.ext.junit.rules.ActivityScenarioRule;

import com.example.pathfinder.R;

import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;

import static androidx.test.espresso.Espresso.onView;
import static androidx.test.espresso.action.ViewActions.click;
import static androidx.test.espresso.intent.matcher.IntentMatchers.hasComponent;
import static androidx.test.espresso.matcher.ViewMatchers.withId;


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
     */
    @Before
    public void setUp() {
        Intents.init();
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


}